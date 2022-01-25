SOURCE_FOLDER = lib
HEADER_FOLDER = include
BUILD_FOLDER = build

SOURCES = $(shell find $(SOURCE_FOLDER)/ | grep "\.cpp$$")
HEADERS = $(shell find $(HEADER_FOLDER)/ | grep "\.hpp$$")
OBJECTS = $(addprefix $(BUILD_FOLDER)/,$(patsubst %.cpp,%.o,$(SOURCES)))

CXX_FLAGS = -fPIC -g -I$(HEADER_FOLDER) -I$(shell python3-config --cflags) -std=c++17
LD_FLAGS = $(shell python3-config --ldflags) -lpython3 -lboost_python3 -ltclstub8.6

PRODUCT = pyinter.so

CC ?= cc
CXX ?= c++

all: $(PRODUCT)

$(OBJECTS): $(BUILD_FOLDER)/%.o:%.cpp $(SOURCES) $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CXX) -c -o $@ $(CXX_FLAGS) $<

$(PRODUCT): $(OBJECTS)
	$(CXX) -shared -o $@ $(LD_FLAGS) $^

.PHONY: smoke_test
smoke_test: $(PRODUCT)
	$(MAKE) -C test/smoke

.PHONY: fmt
fmt:
	clang-format -i $(SOURCES) $(HEADERS)

.PHONY: clean
clean:
	rm -rf __pycache__
	rm -rf $(BUILD_FOLDER)
	rm -f $(PRODUCT)