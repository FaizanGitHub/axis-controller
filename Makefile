CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -g -Isrc -Itest
BUILD := build

test: $(BUILD)
	$(CC) $(CFLAGS) test/test_cmd_pool.c test/unity/unity.c -o $(BUILD)/test_cmd_pool
	./$(BUILD)/test_cmd_pool
	$(CC) $(CFLAGS) test/test_parser.c test/unity/unity.c -o $(BUILD)/test_parser
	./$(BUILD)/test_parser
	$(CC) $(CFLAGS) test/test_motion_profile.c test/unity/unity.c -o $(BUILD)/test_mp
	./$(BUILD)/test_mp

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

.PHONY: test clean
