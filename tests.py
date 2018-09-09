import sys
import time
import ctypes
import unittest
from random import randint

# Load the module and port functions into python
################################################################################
avl_dll = ctypes.CDLL("build/avl.dll")

################################################################################
avl_create_proto = ctypes.CFUNCTYPE (
    ctypes.c_void_p
)
avl_create = avl_create_proto(("avl_create", avl_dll))
################################################################################
avl_add_proto = ctypes.CFUNCTYPE (
    None,
    ctypes.c_void_p,
    ctypes.c_long
)

avl_add = avl_add_proto(("avl_add", avl_dll), (
    (1, "avl_tree", 0),
    (1, "value", 0),
))
################################################################################
avl_exists_proto = ctypes.CFUNCTYPE (
    ctypes.c_bool,
    ctypes.c_void_p,
    ctypes.c_long
)

avl_exists = avl_exists_proto(("avl_exists", avl_dll), (
    (1, "avl_tree", 0),
    (1, "value", 0),
))
################################################################################
avl_destroy_proto = ctypes.CFUNCTYPE (
    None,
    ctypes.c_void_p
)

avl_destroy = avl_destroy_proto(("avl_destroy", avl_dll), (
    (1, "avl_tree", 0),
))
################################################################################
avl_get_raw_proto = ctypes.CFUNCTYPE (
    ctypes.c_void_p,
    ctypes.c_void_p
)

avl_get_raw = avl_get_raw_proto(("avl_get_raw_data", avl_dll), (
    (1, "avl_tree", 0),
))
################################################################################
class Edge(ctypes.Structure):
    _fields_ = [("v", ctypes.c_ulong),
                ("u", ctypes.c_ulong)]

    def __repr__(self):
        return "{0:d}->{1:d}".format(self.u, self.v)

class RawData(ctypes.Structure):
    _fields_ = [("count_edges", ctypes.c_ulong),
                ("edges", ctypes.POINTER(Edge))]
################################################################################

class AvlTree(object):
    def __init__(self):
        self._obj = avl_create()

    def __del__(self):
        avl_destroy(self._obj)

    def add(self, x):
        if not hasattr(x, "__iter__"):
           x = [x] 
        for e in x:
           avl_add(self._obj, hash(e))
        
    def contains(self, x):
        return avl_exists(self._obj, hash(x)) 

    def raw_data(self):
        raw_data_ptr = avl_get_raw(self._obj);
        return ctypes.cast(raw_data_ptr, ctypes.POINTER(RawData))

class TestCollectionProperties(unittest.TestCase):
    
    def setUp(self):
        self._avl = AvlTree()

    def test_empty_collection_does_not_contains_42(self):
        self.assertFalse(self._avl.contains(42))

    def test_one_item_insertion(self):
        self._avl.add(42)
        self.assertTrue(self._avl.contains(42))

    def test_several_item_insertion(self):
        self._avl.add(42)
        self._avl.add(7)
        self._avl.add(1)
        self.assertFalse(self._avl.contains(5))
        self.assertTrue(self._avl.contains(7))
        self.assertTrue(self._avl.contains(1))
        self.assertTrue(self._avl.contains(42))

class TestStress(unittest.TestCase):

    def setUp(self):
        self._avl = AvlTree()
        self._timer = time.time()

    def tearDown(self):
        delta = time.time() - self._timer
        sys.stdout.write("delta time: {:.2f}, ".format(delta))
        sys.stdout.flush()

    def test_thousand_entries(self):
        for i in range(1000):
            self._avl.add(randint(0, 1e6))
    
    def test_ten_thousand_entries(self):
        for i in range(10000):
            self._avl.add(randint(0, 1e6))

    def test_ten_thousand_entries_sequencial(self):
        for i in range(10000):
            self._avl.add(i)

    def test_hundred_thousand_entries(self):
        for i in range(100000):
            self._avl.add(randint(0, 1e6))

    def test_hundred_thousand_entries_sequncial(self):
        for i in range(100000):
            self._avl.add(i)

    def test_million_entries(self):
        for i in range(int(1e6)):
            self._avl.add(randint(0, 1e6))
            
    def test_million_entries_sequncial(self):
        for i in range(int(1e6)):
            self._avl.add(i)

class TestExtractedRawData(unittest.TestCase):

    def setUp(self):
        self._avl = AvlTree()

    def test_raw_data_extraction_wont_crash_empty(self):
        self._avl.raw_data()

    def test_raw_data_extraction_wont_crash(self):
        self._avl.add([6, 11, 5, 2, 4, 3, 8, 4, 6, 3, 15, 32, 1, 0, 7]);
        raw_data = self._avl.raw_data()

if __name__ == "__main__":
    unittest.main(verbosity=2)
