CXX        ?= g++
CXXFLAGS   ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -pthread
TSAN_FLAGS := -std=c++20 -O1 -g -Wall -Wextra -pthread -fsanitize=thread

PROGS := producer_consumer reader_writer dining_philosophers

.PHONY: all run tsan clean

all: $(PROGS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# ThreadSanitizer builds (*.tsan) — any data race is reported at runtime.
# Dining-philosophers solutions run one per process: std::mutex has a trivial
# destructor on libstdc++, so TSan would treat run 1's forks and run 3's forks
# (same stack addresses) as the same mutexes and report a bogus lock-order cycle.
# The waiter solution runs with lock-order checking off: its lock order really is
# cyclic, and only the N-1 semaphore (invisible to TSan) prevents the deadlock.
# Data-race detection stays on for it.
tsan: $(PROGS:=.tsan)
	./producer_consumer.tsan
	./reader_writer.tsan
	for s in ordered scoped monitor; do ./dining_philosophers.tsan $$s || exit 1; done
	TSAN_OPTIONS=detect_deadlocks=0 ./dining_philosophers.tsan waiter

%.tsan: %.cpp
	$(CXX) $(TSAN_FLAGS) $< -o $@

run: all
	./producer_consumer && ./reader_writer && ./dining_philosophers

clean:
	rm -f $(PROGS) $(PROGS:=.tsan)
