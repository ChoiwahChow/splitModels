CXXFLAGS = -g -Wall -fmessage-length=0
# CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		splitModels.o Invariant.o Interpretation.o TreeNode.o

LIBS =

TARGET =	splitModels

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
