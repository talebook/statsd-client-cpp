
all:
	make -C src/;
	make -C demo/;
	@for f in demo/*; do test -x $$f && cp -v $$f ./; done; echo

clean:
	make -C src/ clean;
	make -C demo/ clean;
	@rm -f ./system_monitor ./test_client

