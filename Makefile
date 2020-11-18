.PHONY: build flash menuconfig

build flash menuconfig monitor:
	@bundle exec rake $@
