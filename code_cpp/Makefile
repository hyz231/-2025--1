CPP=g++
CXXFLAGS=-std=c++17 -O2 -I./src -I./tests
SRCS=$(wildcard src/*.cpp)
# 测试时排除 main.cpp，因为 gtest_main 会提供 main 函数
TEST_SRCS=$(filter-out src/main.cpp,$(SRCS))
OBJS=$(SRCS:.cpp=.o)
BIN=bin/ledger.exe

# GTest flags
# 自动检测 GTest 安装路径（支持多个常见位置）
GTEST_INCLUDE := -I/usr/include
GTEST_LIB_DIRS := -L/usr/lib/x86_64-linux-gnu -L/usr/lib -L/usr/local/lib
GTEST_LIBS := $(GTEST_LIB_DIRS) -lgtest -lgtest_main -lpthread

# 检查头文件是否存在
ifeq ($(wildcard /usr/include/gtest/gtest.h),)
    ifneq ($(wildcard /usr/local/include/gtest/gtest.h),)
        GTEST_INCLUDE := -I/usr/local/include
    else
        $(warning GTest header not found. Please install: sudo apt-get install libgtest-dev)
    endif
endif

# 尝试使用 pkg-config（如果可用）
ifneq ($(shell pkg-config --exists gtest 2>/dev/null && echo yes),)
    GTEST_INCLUDE := $(shell pkg-config --cflags gtest)
    GTEST_LIBS := $(shell pkg-config --libs gtest) -lpthread
endif

# Test binaries
TEST_STORAGE_BIN=bin/test_storage_gtest.exe
TEST_SEARCH_BIN=bin/test_search_gtest.exe
TEST_INTEGRATION_BIN=bin/test_integration.exe
TEST_DEFECTS_BIN=bin/test_defects.exe

all: $(BIN)

$(BIN): $(SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) -o $(BIN) $(SRCS)

# GTest tests
test-storage: $(TEST_STORAGE_BIN)
	@echo "Running Storage tests..."
	./$(TEST_STORAGE_BIN)

test-search: $(TEST_SEARCH_BIN)
	@echo "Running Search tests..."
	./$(TEST_SEARCH_BIN)

test-integration: $(TEST_INTEGRATION_BIN)
	@echo "Running Integration tests..."
	./$(TEST_INTEGRATION_BIN)

test-defects: $(TEST_DEFECTS_BIN)
	@echo "Running defect detection tests..."
	./$(TEST_DEFECTS_BIN)

test-all: test-storage test-search test-integration test-defects
	@echo "All tests completed!"

# Build test binaries
# 注意：使用 TEST_SRCS 而不是 SRCS，排除 main.cpp
$(TEST_STORAGE_BIN): tests/test_storage_gtest.cpp $(TEST_SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) $(GTEST_INCLUDE) -o $(TEST_STORAGE_BIN) tests/test_storage_gtest.cpp $(TEST_SRCS) $(GTEST_LIBS)

$(TEST_SEARCH_BIN): tests/test_search_gtest.cpp $(TEST_SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) $(GTEST_INCLUDE) -o $(TEST_SEARCH_BIN) tests/test_search_gtest.cpp $(TEST_SRCS) $(GTEST_LIBS)

$(TEST_INTEGRATION_BIN): tests/test_integration.cpp $(TEST_SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) $(GTEST_INCLUDE) -o $(TEST_INTEGRATION_BIN) tests/test_integration.cpp $(TEST_SRCS) $(GTEST_LIBS)

$(TEST_DEFECTS_BIN): tests/test_defects.cpp $(TEST_SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) $(GTEST_INCLUDE) -o $(TEST_DEFECTS_BIN) tests/test_defects.cpp $(TEST_SRCS) $(GTEST_LIBS)

# Original test
test-storage-original: tests/test_storage.cpp $(SRCS)
	@mkdir -p bin
	$(CPP) $(CXXFLAGS) -o bin/test_storage.exe tests/test_storage.cpp $(SRCS)
	./bin/test_storage.exe

clean:
	rm -f $(BIN) src/*.o bin/*.exe
	rm -rf tmp_test_* tmp_new_dir custom_data

.PHONY: all test-storage test-search test-all test-integration test-defects test-storage-original clean
