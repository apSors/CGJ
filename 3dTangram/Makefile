CXX := clang++

ENGINE := mgl
ENGINEDIR := ../$(ENGINE)

INCLUDES := \
	-I/usr/include \
	-I$(ENGINEDIR)

LIBS := \
	-L/usr/lib -lOpenGL -lglfw -lGLEW -lassimp \
	-L$(ENGINEDIR) -l$(ENGINE)

OUT := hello-3d-world

all : release

release : CXXFLAGS := -O2 -D NDEBUG
release : $(OUT)

debug : CXXFLAGS := -g -Wall -D DEBUG
debug : $(OUT)

$(OUT) : $(OUT).o $(ENGINEDIR)/lib$(ENGINE).so
	$(CXX) $(LIBS) -o $@ $<

$(OUT).o : $(OUT).cpp $(ENGINEDIR)/$(ENGINE).hpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $<

clean :
	$(RM) *.o $(OUT)

run :
	LD_LIBRARY_PATH=$(ENGINEDIR) ./$(OUT)
