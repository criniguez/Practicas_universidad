# _________________________________________________________________________________________
# Intel.ligencia Artificial
# Grau en Enginyeria Informatica
# Curs 2021 - 2022
# Universitat Autonoma de Barcelona
# _______________________________________________________________________________________

from SubwayMap import *
from utils import *
import os
import math
import copy


def expand(path, mapo):
    """
     It expands a SINGLE station and returns the list of class Path.
     Format of the parameter is:
        Args:
            path (object of Path class): Specific path to be expanded
            map (object of Map class): All the information needed to expand the node
        Returns:s
            path_list (list): List of paths that are connected to the given path.
    """
    l = []
    for i in mapo.connections[path.last]:
        aux = copy.deepcopy(path)
        aux.add_route(i)
        l.append(aux)
    return l


def remove_cycles(path_list):
    """
     It removes from path_list the set of paths that include some cycles in their path.
     Format of the parameter is:
        Args:
            path_list (LIST of Path Class): Expanded paths
        Returns:
            path_list (list): Expanded paths without cycles.
    """
    return [i for i in path_list if len(set(i.route)) == len(i.route)]


def insert_depth_first_search(expand_paths, list_of_path):
    """
     expand_paths is inserted to the list_of_path according to DEPTH FIRST SEARCH algorithm
     Format of the parameter is:
        Args:
            expand_paths (LIST of Path Class): Expanded paths
            list_of_path (LIST of Path Class): The paths to be visited
        Returns:
            list_of_path (LIST of Path Class): List of Paths where Expanded Path is inserted
    """
    return expand_paths + list_of_path


def depth_first_search(origin_id, destination_id, mapo):
    """
     Depth First Search algorithm
     Format of the parameter is:
        Args:
            origin_id (int): Starting station id
            destination_id (int): Final station id
            map (object of Map class): All the map information
        Returns:
            list_of_path[0] (Path Class): the route that goes from origin_id to destination_id
    """
    l = [Path(origin_id)]
    while True: 
        path = l[0]
        e = expand(path, mapo)
        e = remove_cycles(e)
        l = insert_depth_first_search(e, l[1:])
        if l == [] or l[0].last == destination_id:
            break
    if l != []:
        return l[0]
    return "No existeix Solucio"


def insert_breadth_first_search(expand_paths, list_of_path):
    """
        expand_paths is inserted to the list_of_path according to BREADTH FIRST SEARCH algorithm
        Format of the parameter is:
           Args:
               expand_paths (LIST of Path Class): Expanded paths
               list_of_path (LIST of Path Class): The paths to be visited
           Returns:
               list_of_path (LIST of Path Class): List of Paths where Expanded Path is inserted
    """
    return list_of_path + expand_paths


def breadth_first_search(origin_id, destination_id, mapo):
    """
     Breadth First Search algorithm
     Format of the parameter is:
        Args:
            origin_id (int): Starting station id
            destination_id (int): Final station id
            map (object of Map class): All the map information
        Returns:
            list_of_path[0] (Path Class): The route that goes from origin_id to destination_id
    """
    l = [Path(origin_id)]
    while True: 
        path = l[0]
        e = expand(path, mapo)
        e = remove_cycles(e)
        l = insert_breadth_first_search(e, l[1:])
        if l == [] or l[0].last == destination_id:
            break
    if l != []:
        return l[0]
    return "No existeix Solucio"


def calculate_cost(expand_paths, mapo, type_preference=0):
    """
         Calculate the cost according to type preference
         Format of the parameter is:
            Args:
                expand_paths (LIST of Paths Class): Expanded paths
                map (object of Map class): All the map information
                type_preference: INTEGER Value to indicate the preference selected:
                                0 - Adjacency
                                1 - minimum Time
                                2 - minimum Distance
                                3 - minimum Transfers
            Returns:
                expand_paths (LIST of Paths): Expanded path with updated cost
    """
    if(type_preference == 0):
        for i in expand_paths: 
            i.update_g(1)
    
    if(type_preference == 1): 
        for i in expand_paths:
            i.update_g(mapo.connections[i.penultimate][i.last])
    
    if(type_preference == 2):
        for i in expand_paths:
            if mapo.stations[i.penultimate]['name'] != mapo.stations[i.last]['name']:
                i.update_g(mapo.connections[i.penultimate][i.last] * mapo.stations[i.penultimate]['velocity'])

    if(type_preference == 3):
        for i in expand_paths:
            i.update_g(mapo.stations[i.penultimate]['name'] == mapo.stations[i.last]['name'])

    return expand_paths



def insert_cost(expand_paths, list_of_path):
    """
        expand_paths is inserted to the list_of_path according to COST VALUE
        Format of the parameter is:
           Args:
               expand_paths (LIST of Path Class): Expanded paths
               list_of_path (LIST of Path Class): The paths to be visited
           Returns:
               list_of_path (LIST of Path Class): List of Paths where expanded_path is inserted according to cost
    """
    return sorted(list_of_path + expand_paths, key=lambda x: x.g)
    


def uniform_cost_search(origin_id, destination_id, mapo, type_preference=0): 
    """
     Uniform Cost Search algorithm
     Format of the parameter is:
        Args:
            origin_id (int): Starting station id
            destination_id (int): Final station id
            map (object of Map class): All the map information
        Returns:
            list_of_path[0] (Path Class): The route that goes from origin_id to destination_id
    """
    l = [Path(origin_id)]
    while True: 
        path = l[0]
        e = expand(path, mapo)
        e = remove_cycles(e)
        e = calculate_cost(e, mapo, type_preference)
        l = insert_cost(e, l[1:])
        if l == [] or l[0].last == destination_id:
            break
    if l != []:
        return l[0]
    return "No existeix Solucio"


def calculate_heuristics(expand_paths, mapo, destination_id, type_preference=0):
    """
     Calculate and UPDATE the heuristics of a path according to type preference
     WARNING: In calculate_cost, we didn't update the cost of the path inside the function
              for the reasons which will be clear when you code Astar (HINT: check remove_redundant_paths() function).
     Format of the parameter is:
        Args:
            expand_paths (LIST of Path Class): Expanded paths
            map (object of Map class): All the map information
            type_preference: INTEGER Value to indicate the preference selected:
                            0 - Adjacency
                            1 - minimum Time
                            2 - minimum Distance
                            3 - minimum Transfers
        Returns:
            expand_paths (LIST of Path Class): Expanded paths with updated heuristics
    """
    if(type_preference == 0):
        for i in expand_paths: 
            i.update_h(i.last != destination_id)
    
    if(type_preference == 1): 
        for i in expand_paths:
            max_v = max([v['velocity'] for _,v in mapo.stations.items()])
            aux = euclidean_dist( 
                    (mapo.stations[destination_id]['x'], mapo.stations[destination_id]['y']), 
                    (mapo.stations[i.last]['x'], mapo.stations[i.last]['y']) 
            )/max_v
            i.update_h(aux)
    
    if(type_preference == 2):
        for i in expand_paths:
            i.update_h(euclidean_dist( 
                    (mapo.stations[destination_id]['x'], mapo.stations[destination_id]['y']), 
                    (mapo.stations[i.last]['x'], mapo.stations[i.last]['y'])
            ))
    
    
    if(type_preference == 3):
        for i in expand_paths: 
            i.update_h(mapo.stations[i.last]['line'] != mapo.stations[destination_id]['line'])

    return expand_paths

def update_f(expand_paths):
    """
      Update the f of a path
      Format of the parameter is:
         Args:
             expand_paths (LIST of Path Class): Expanded paths
         Returns:
             expand_paths (LIST of Path Class): Expanded paths with updated costs
    """
    for i in expand_paths: i.update_f()
    return expand_paths


def remove_redundant_paths(expand_paths, list_of_path, visited_stations_cost):
    """
      It removes the Redundant Paths. They are not optimal solution!
      If a station is visited and have a lower g in this moment, we should remove this path.
      Format of the parameter is:
         Args:
             expand_paths (LIST of Path Class): Expanded paths
             list_of_path (LIST of Path Class): All the paths to be expanded
             visited_stations_cost (dict): All visited stations cost
         Returns:
             new_paths (LIST of Path Class): Expanded paths without redundant paths
             list_of_path (LIST of Path Class): list_of_path without redundant paths
    """
    for i in expand_paths:    
        if i.last in visited_stations_cost:
            if i.g < visited_stations_cost[i.last]:
                list_of_path = list(filter(lambda x: x.g != i.g and x.last != i.last, list_of_path))
                visited_stations_cost[i.last] = i.g
            else:
                expand_paths.remove(i)
        else:
            visited_stations_cost[i.last] = i.g
    return expand_paths, list_of_path, visited_stations_cost


def insert_cost_f(expand_paths, list_of_path):
    """
        expand_paths is inserted to the list_of_path according to f VALUE
        Format of the parameter is:
           Args:
               expand_paths (LIST of Path Class): Expanded paths
               list_of_path (LIST of Path Class): The paths to be visited
           Returns:
               list_of_path (LIST of Path Class): List of Paths where expanded_path is inserted according to f
    """
    return sorted(list_of_path + expand_paths, key=lambda x:x.f)


def coord2station(coord, mapo):
    """
        From coordinates, it searches the closest station.
        Format of the parameter is:
        Args:   
            coord (list):  Two REAL values, which refer to the coordinates of a point in the city.
            map (object of Map class): All the map information
        Returns:
            possible_origins (list): List of the Indexes of stations, which corresponds to the closest station
    """
    aux = [(euclidean_dist(coord, (v['x'], v['y'])), k) for k,v in mapo.stations.items()]
    min_dist = min(aux, key=lambda x: x[0])[0]
    return [j for i,j in aux if i == min_dist]

def Astar(origin_coor, dest_coor, mapo, type_preference=0):
    """
     A* Search algorithm
     Format of the parameter is:
        Args:
            origin_id (list): Starting station id
            destination_id (int): Final station id
            map (object of Map class): All the map information
            type_preference: INTEGER Value to indicate the preference selected:
                            0 - Adjacency
                            1 - minimum Time
                            2 - minimum Distance
                            3 - minimum Transfers
        Returns:
            list_of_path[0] (Path Class): The route that goes from origin_id to destination_id
    """  

    origin_id  = coord2station(origin_coor, mapo)
    dest_id = coord2station(dest_coor, mapo)[0]
    l = [Path(i) for i in origin_id]
    visited_stations_cost = {}
    while True: 
        path = l[0]
        e = expand(path, mapo)
        e = remove_cycles(e)
        e = calculate_cost(e, mapo, type_preference)
        e, l, visited_stations_cost = remove_redundant_paths(e, l, visited_stations_cost)
        e = calculate_heuristics(e, mapo, dest_id, type_preference)
        e = update_f(e)
        l = insert_cost_f(e, l[1:])
        if l == [] or l[0].last == dest_id:
            break
    if l != []:
        return l[0]
    return "No existeix Solucio"


def aux_fuction(coord, mapo):
    l = []

    for i,v in mapo.stations.items():
        aux = Path(i)
        dist = euclidean_dist(coord, (v['x'], v['y']))/5
        aux.update_g(dist)
        aux.update_f()
        l.append(aux)
    
    l = sorted(l, key=lambda x: x.g)
    return l


def Astar_improved(origin_coor, dest_coor, mapo): 
    l = aux_fuction(origin_coor, mapo)
    dest_id = coord2station(dest_coor, mapo)[0]
    visited_stations_cost = {}

    while True: 
        path = l[0]
        e = expand(path, mapo)
        e = remove_cycles(e)
        e = calculate_cost(e, mapo, 1)
        e, l, visited_stations_cost = remove_redundant_paths(e, l, visited_stations_cost)
        e = calculate_heuristics(e, mapo, dest_id, 1)
        e = update_f(e)
        l = insert_cost_f(e, l[1:])
        if l == [] or l[0].last == dest_id:
            break 
    
    if l != []:
        return l[0]
    return "No existeix Solucio"