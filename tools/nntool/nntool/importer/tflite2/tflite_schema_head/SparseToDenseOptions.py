# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class SparseToDenseOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = SparseToDenseOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsSparseToDenseOptions(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    @classmethod
    def SparseToDenseOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # SparseToDenseOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # SparseToDenseOptions
    def ValidateIndices(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

def SparseToDenseOptionsStart(builder): builder.StartObject(1)
def Start(builder):
    return SparseToDenseOptionsStart(builder)
def SparseToDenseOptionsAddValidateIndices(builder, validateIndices): builder.PrependBoolSlot(0, validateIndices, 0)
def AddValidateIndices(builder, validateIndices):
    return SparseToDenseOptionsAddValidateIndices(builder, validateIndices)
def SparseToDenseOptionsEnd(builder): return builder.EndObject()
def End(builder):
    return SparseToDenseOptionsEnd(builder)