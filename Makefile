# Proiect SSC P4 - vectorizare cu instructiuni SIMD
#
#   make            compileaza tot ce exista
#   make check-cpu  afiseaza extensiile SIMD disponibile
#   make smoke      ruleaza testele minimale, cate unul per backend
#   make test       verifica modulul src/common
#   make run        ruleaza benchmark-ul (masuratorile se adauga in Lab 4)
#   make plot       genereaza graficele (din Lab 6)
#   make asm        genereaza assembly-ul in build/asm
#   make clean      sterge build/

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Isrc
BUILD    := build

# Fiecare director este compilat doar cu flag-urile lui
BACKENDS     := scalar sse avx2 avx512
FLAGS_common := -O2
FLAGS_scalar := -O2 -fno-tree-vectorize
FLAGS_sse    := -O2 -msse4.2
FLAGS_avx2   := -O2 -mavx2 -mfma
FLAGS_avx512 := -O2 -mavx512f -mavx512vl -mavx512bw -mavx512dq

# Comenzile difera intre cmd.exe si shell-ul de pe Linux
ifeq ($(OS),Windows_NT)
    SHELL  := cmd.exe
    EXE    := .exe
    MKDIR   = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
    RMDIR   = if exist "$1" rmdir /s /q "$1"
    RUNBIN  = $(subst /,\,$1)
else
    EXE    :=
    MKDIR   = mkdir -p "$1"
    RMDIR   = rm -rf "$1"
    RUNBIN  = ./$1
endif

CHECK_CPU := $(BUILD)/check_cpu$(EXE)
BENCH_BIN := $(BUILD)/bench$(EXE)
TEST_BIN  := $(BUILD)/test_common$(EXE)

COMMON_OBJ  := $(patsubst src/%.cpp,$(BUILD)/%.o,$(wildcard src/common/*.cpp))
BACKEND_SRC := $(filter-out %/smoke.cpp,$(foreach b,$(BACKENDS),$(wildcard src/$(b)/*.cpp)))
BACKEND_OBJ := $(patsubst src/%.cpp,$(BUILD)/%.o,$(BACKEND_SRC))

# Un backend are smoke test din momentul in care exista src/<backend>/smoke.cpp
SMOKE_SRC := $(wildcard src/*/smoke.cpp)
SMOKE_BIN := $(patsubst src/%/smoke.cpp,$(BUILD)/smoke_%$(EXE),$(SMOKE_SRC))

ASM_OUT := $(foreach b,$(BACKENDS),$(patsubst src/$(b)/%.cpp,$(BUILD)/asm/$(b)/%.s,$(wildcard src/$(b)/*.cpp)))

.PHONY: all run plot clean check-cpu smoke test asm
.SECONDARY:

all: $(CHECK_CPU) $(SMOKE_BIN) $(BENCH_BIN) $(TEST_BIN)

run: $(BENCH_BIN)
	$(call RUNBIN,$(BENCH_BIN))

plot:
	@echo Graficele se adauga in Lab 6.

check-cpu: $(CHECK_CPU)
	$(call RUNBIN,$(CHECK_CPU))

smoke: $(SMOKE_BIN)
	$(foreach b,$(SMOKE_BIN),$(call RUNBIN,$(b)) &&) echo Smoke test finalizat.

test: $(TEST_BIN)
	$(call RUNBIN,$(TEST_BIN))

asm: $(ASM_OUT)

clean:
	$(call RMDIR,$(BUILD))

$(CHECK_CPU): bench/check_cpu.cpp
	@$(call MKDIR,$(@D))
	$(CXX) $(CXXFLAGS) $(FLAGS_common) $< -o $@

$(BENCH_BIN): $(BUILD)/bench/main.o $(COMMON_OBJ) $(BACKEND_OBJ)
	$(CXX) $^ -o $@

$(TEST_BIN): $(BUILD)/bench/test_common.o $(COMMON_OBJ) $(BACKEND_OBJ)
	$(CXX) $^ -o $@

$(BUILD)/smoke_%$(EXE): $(BUILD)/%/smoke.o
	$(CXX) $< -o $@

$(BUILD)/bench/%.o: bench/%.cpp
	@$(call MKDIR,$(@D))
	$(CXX) $(CXXFLAGS) $(FLAGS_common) -MMD -MP -c $< -o $@

define BACKEND_RULES
$(BUILD)/$(1)/%.o: src/$(1)/%.cpp
	@$$(call MKDIR,$$(@D))
	$$(CXX) $$(CXXFLAGS) $$(FLAGS_$(1)) -MMD -MP -c $$< -o $$@

$(BUILD)/asm/$(1)/%.s: src/$(1)/%.cpp
	@$$(call MKDIR,$$(@D))
	$$(CXX) $$(CXXFLAGS) $$(FLAGS_$(1)) -S $$< -o $$@
endef

$(foreach b,common $(BACKENDS),$(eval $(call BACKEND_RULES,$(b))))

-include $(wildcard $(BUILD)/*/*.d)
