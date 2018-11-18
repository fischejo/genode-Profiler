#!/usr/bin/env python3

"""profilerlib 

This library contains classes and methods for working with .profile files. This
files are created by `profiler-filter` and contain normal json. 

"""
__author__ = "Johannes Fischer"
__license__ = "GNU Affero General Public License"
__version__ = "1.0"
__maintainer__ = "Johannes Fischer"
__status__ = "Production"

import itertools
import json
import re
import collections
import argparse
import statistics 
import sys

class Instance:
    def __init__(self, start, stop, color):
        """Initialize a Instance Object

        A Instance Object holds information about a single execution of a Scope.

        :param start int: Start of the execution
        :param stop int: Start of the execution
        :param color str: Color used by matplotlib in order to plot the bar.
        """        
        self.start = start
        self.stop = stop
        self.color = color

    def __str__(self):
        return "<Instance({} us - {} us in {})>".format(self.start, self.stop, self.color)

    def _mean(instances, ignore_color_disparity=False):
        start = statistics.mean([instance.start for instance in instances])
        stop = statistics.mean([instance.stop for instance in instances])
        color = instances[0].color
        same_color = all([instance.color == color for instance in instances])
        if not same_color and not ingore_color_disparity:
            raise ValueError("A merged pair of instances differs in colors.")

        return Instance(start, stop, color)
        
    @property
    def duration(self):
        """Duration of this Instance
        
        :return: The duration of this Instance
        :rtype: int
        """
        return self.stop-self.start

    
class Scope(collections.Sequence):
    def _pretty(name):
        """Extracts the function name, if the scope name is determined with C++'s
        __PRETTY_FUNCTION__ macro.

        :param name str: Full name of the Scope
        :return: Name of the scope
        :rtype: str
        """        
        pretty = re.match(r'^(.*) (.*)\((.*)\)', name)
        return pretty.group(2) if pretty else name

    
    def __init__(self, name, instances):
        """Initialize a Scope Object

        This class provides read-only methods for querying information about a
        specific scope in the profile file.

        :param name str: Name of the Scope
        :param instances [Instance]: Instances hold by this Scope

        All instances hold by a Scope are sorted according to the start time.
        """
        self._pretty = Scope._pretty(name)
        self._name = name
        self._instances = sorted(instances, key=lambda i:i.start)
        durations = map(lambda instance: instance.duration, instances)
        self._average = int(sum(list(durations))/len(instances))

        
    def __len__(self):
        """Number of instances hold by this Scope

        :return: Number of instances hold by this Scope
        :rtype: int
        """
        return len(self._instances)


    def __getitem__(self, index):
        """Returns Instance at `index` of the instance list.

        :param index int: position of the instance in the list
        :return: Instance object
        :rtype: Instance
        """        
        return self._instances.__getitem__(index)


    @property
    def average(self):
        """Average runtime of this Scope

        :return: Average runtime of this Scope
        :rtype: intp
        """                
        return self._average


    @property
    def pretty(self):
        """The pretty name of this Scope. 

        If the name of the scope is determined by the C++'s __PRETTY_NAME__
        macro, this method will only return the function name without return
        value and parameters.

        :return: Pretty name of the Scope
        :rtype: str
        """
        return self._pretty

    
    def __str__(self):
        """The full name of this Scope. 

        :return: Full name of the Scope
        :rtype: str
        """        
        return self._name

    
    @property
    def name(self):
        """The full name of this Scope. 

        :return: Full name of the Scope
        :rtype: str
        """                
        return self._name

    
    def _mean(scopes, ignore_color_disparity=False):
        # check if all scopes have the same name, otherwise merging doesn't make
        # sense.
        scope_str = str(scopes[0])
        same_str = all([str(scope) == scope_str for scope in scopes])
        if not same_str:
            raise ValueError("Merging of scopes not possible as not "\
                             "every sope has the same name")

        # check if all scopes have the same number of instances
        instance_count = len(scopes[0])
        same_count = all([len(scope) == instance_count for scope in scopes])
        if not same_count:
            raise ValueError("Not every scope has the same number of instances.")

        merged_instances = []
        # select instances which are at position i of the instance list
        # this works because the instance list is ordered by start time.
        for i in range(instance_count):
            merged_instance = Instance._mean([scope[i] for scope in scopes],
                                             ignore_color_disparity)
            merged_instances.append(merged_instance)

        # return new Scope with old name and merged instances.
        return Scope(scope_str, merged_instances)
        
    
class Profile(collections.Mapping):
    __slots__ = ['_scopes']

    def load(self, path):

        """Load scopes form a profile file
        :param path str: Path to the profile file
        """
        self._scopes = collections.defaultdict(list)

        # parse json input
        json_instances = json.load( open(path, 'r'))
        for json_instance  in json_instances:
            name = json_instance['name']
            start = json_instance['start']
            stop = json_instance['stop']
            color = json_instance['color']
            self._scopes[name].append(Instance(start, stop, color))

        # wrap the list of Instance objects into a Scope
        for name,instances in self._scopes.items():
            self._scopes[name] = Scope(name, instances)

    def __init__(self, scopes=[]):
        """Initialize a Profile Object

        With help of this class, it is possible to load and save .profile
        files. It provides read-only dictionary-like methods for querying
        information about the loaded profile file.

        :param scopes [Scope]: Optionally handle over a list of scopes

        """
        self._scopes = collections.defaultdict(list)
        for scope in scopes:
            self._scopes[str(scope)] = scope


    def __getitem__(self, key):
        """Get Scope by Name
        :param key str: Full name of the Scope
        :return: Scope with this name
        :rtype: Scope
        """                
        return self._scopes[key]


    def __len__(self):
        """Number of scopes in this profile
    
        :return: Number of scopes
        :rtype: Int
        """    
        return len(self._scopes)


    def __iter__(self):
        """Returns a iterator of the internal dictionary structure for scopes
    
        :return: Iterator of a dictionary 
        :rtype: Iterator
        """        
        return iter(self._scopes)


    def getsorted(self):
        """Returns a list of sorted scopes
    
        :return: list of sorted scopes according to the start time of the first instance.
        :rtype: [Scope, Scope, ...]
        """        
        return sorted(self._scopes.values(), reverse=True,
                      key=lambda scope:scope[0].start)


    def save(self, path):
        """Saves this profile to a file
    
        :param path str: Path to output file.
        """        
        output = []
        for name, scope in self._scopes.items():
            for instance in scope:
                output.append({
                    'name' : name,
                    'start': instance.start,
                    'stop': instance.stop,                        
                    'color': instance.color
                })
        with open(path, 'a') as f:
            f.write(json.dumps(output, ensure_ascii=False))


    def mean(profiles, ingore_scope_disparity=False, ignore_color_disparity=False):
        """Creates a mean Profile from multiple Profiles
    
        :param profiles [Profile]: A list of profiles which should be merged.

        :param ignore_scope_disparity Bool: Merge scopes which are not present
        in every profile.
        
        :param ignore_color_disparity Bool: Ignore differences in color of
        merged instances.

        :return: A profile where all instances of every scope are merged by
        mean.

        If `ignore_color_disparity` is enabled, the color of the first instance
        of the scope is selected.

        """                    
        # gather all scope names from profiles.
        scope_names = profiles[0].keys()
        for profile in profiles[1:0]:
            for name,scope in profile.getitems():
                if name not in scope_names:
                    if not ignore_scope_disparity:
                        raise ValueError("Scope `{}` is not in every profile.")
                    else:
                        scope_names.append(name)

        # merge scopes from alle profiles
        merged_scopes = []
        for name in scope_names:
            merged_scope = Scope._mean([profile[name] for profile in profiles],
                                       ignore_color_disparity)
            merged_scopes.append(merged_scope)

        return Profile(merged_scopes)
