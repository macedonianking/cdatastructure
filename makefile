
.PHONY: all
all:
	gn gen out
	ninja -C out

.PHONY: clean
clean:
	gn clean out

.PHONY: clone
clone:
	git branch -D main
	git checkout -b main


