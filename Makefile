CXXFLAGS = -g -Wall -fmessage-length=0
# CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		splitModels.o Buckets.o Invariant.o RandomInvariants.o IsoFilter.o Interpretation.o Utils.o ArgumentParser.o InvariantsStore.o TreeNode.o

LIBS =

TARGET =	splitModels

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
