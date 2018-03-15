
.PHONY: all
all:
	gn gen out
	ninja -C out

.PNONY: clean
clean:
	gn clean out
