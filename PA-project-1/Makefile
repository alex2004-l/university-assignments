ARCHIVE := 321CD_LacheAlexandraFlorentinaGeorgiana_Tema1.zip

CCFLAGS := -std=c++17 -Wall -Wextra -O0 -lm

SRC_DIR := .
OUT_DIR := .

CC_SRC := $(wildcard $(SRC_DIR)/*.cpp)
CC_EXECS := $(CC_SRC:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%)

TARGETS := $(CC_EXECS)

.PHONY: build clean pack

build: $(TARGETS)

clean:
	rm -f $(TARGETS) $(OUT_DIR)/*.class $(ARCHIVE)

pack:
	@find $(SRC_DIR) \
		\( -path "./_utils/*" -prune \) -o \
		-regex ".*\.\(cpp\|h\|hpp\|java\)" -exec zip $(ARCHIVE) {} +
	@zip $(ARCHIVE) Makefile
	@[ -f README.md ] && zip $(ARCHIVE) README.md \
		|| echo "You should write README.md!"
	@echo "Created $(ARCHIVE)"

run-p1:
	$(OUT_DIR)/servere
run-p2:
	$(OUT_DIR)/colorare
run-p3:
	$(OUT_DIR)/compresie
run-p4:
	$(OUT_DIR)/criptat
run-p5:
	$(OUT_DIR)/oferta

$(CC_EXECS): $(OUT_DIR)/%: $(SRC_DIR)/%.cpp
	g++ -o $@ $^ $(CCFLAGS)


