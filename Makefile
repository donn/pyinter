CC ?= cc
CXX ?= c++

SOURCE_FOLDER = lib
HEADER_FOLDER = include
BUILD_FOLDER = build

SOURCES = $(shell find $(SOURCE_FOLDER)/ | grep "\.cpp$$")
HEADERS = $(shell find $(HEADER_FOLDER)/ | grep "\.hpp$$")
OBJECTS = $(addprefix $(BUILD_FOLDER)/,$(patsubst %.cpp,%.o,$(SOURCES)))

CXX_FLAGS = $(shell python3-config --cflags) -fPIC -g -I$(HEADER_FOLDER) -std=c++17
LD_FLAGS = $(shell python3-config --ldflags) -lpython3 -lboost_python3 -ltclstub8.6

PRODUCT = pyinter.so

TESTS = $(patsubst test/%,%, $(shell find test/ -mindepth 1 -maxdepth 1))
TEST_TARGETS = $(addprefix test-,$(TESTS))

all: $(PRODUCT)

$(OBJECTS): $(BUILD_FOLDER)/%.o:%.cpp $(SOURCES) $(HEADERS) Makefile
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $(CXX_FLAGS) $<

$(PRODUCT): $(OBJECTS)
	$(CXX) -shared -o $@ $(LD_FLAGS) $^

$(TEST_TARGETS): test-% : $(PRODUCT)
	@echo ">>> Running test '$*'..."
	cd test/$* && tclsh ./test.tcl

test: $(TEST_TARGETS)

.PHONY: fmt
fmt:
	clang-format -i $(SOURCES) $(HEADERS)

.PHONY: clean
clean:
	rm -rf __pycache__
	rm -rf $(BUILD_FOLDER)
	rm -f $(PRODUCT)