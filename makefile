CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -std=c++0x  -I /usr/include/boost
deps = $(shell find ./ -name "*.hpp")
src = $(shell find ./ -name "*.cpp")
obj = $(src:%.cpp=%.o) 


# Which Boost modules to use (all)
BOOST_MODULES = \
  date_time     \
  filesystem    \
  graph         \
  iostreams     \
  math_c99      \
  system        \
  serialization \
  regex

BOOST_LDFLAGS := $(addprefix -lboost_,$(BOOST_MODULES_LIBS))

# Feed compiler/linker flags with Boost's
#CPPFLAGS += $(BOOST_CPPFLAGS)
LDFLAGS += $(BOOST_LDFLAGS)

all: webserver

webserver: $(obj)
	$(CXX) -o webserver $(obj) $(LDFLAGS)

%.o: %.c $(deps)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *.o 