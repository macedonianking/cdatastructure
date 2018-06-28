
.PHONY: all
all:
	gn gen out
	ninja -C out

.PHONY: clean
clean:
	gn clean out
