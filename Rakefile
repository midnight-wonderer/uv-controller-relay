require 'posix/spawn'
require 'securerandom'
require 'shellwords'

MAKE_COMMAND = 'make'
PLATFORM_MAKEFILE = 'esp-platform.mk'
PWD = ::Pathname.new('.')
BUILD_DIR = PWD.join('build')
ESP_BUILD_DIR = BUILD_DIR.join('esp-platform')
APPLICATION_BUILD_DIR = BUILD_DIR.join('application')
APPLICATION_INCLUDE_PATHS = %w[.]

application_source_paths = [
  *::Dir[PWD.join('entries', '**', '*.c')],
  *::Dir[PWD.join('osal', '**', '*.c')],
  *::Dir[PWD.join('modules', '**', '*.c')],
]
application_object_paths = application_source_paths.map do |path|
  target = path.sub(/\A(?:#{
  ::Regexp.quote(PWD.to_s)
  })?\/*(\S*\.c)\z/, '\1.o')
  ::APPLICATION_BUILD_DIR.join(target)
end

class VariableImporter
  VAR_SPLITTER = ::SecureRandom.uuid

  def cc
    @_cc ||= esp_import('CC').strip
  end

  def cc_params
    @_cc_params ||= esp_import('CC_PARAMS')
      .sub(/-D__ESP_FILE__.*\s*(-D)/, '\1')
      .gsub(/\s+(-I)\s+/, ' \1')
      .strip
  end

  def ar
    @_ar ||= esp_import('AR').strip
  end

  def output_elf
    @_output_elf ||= esp_import('APP_ELF').strip
  end

  private

  def esp_import(variable_name)
    ::POSIX::Spawn::Child.new(
      ::MAKE_COMMAND,
      '-f',
      ::PLATFORM_MAKEFILE,
      '--no-print-directory',
      "var-#{variable_name}",
      {
        env: {
          'BUILD_DIR_BASE' => ::ESP_BUILD_DIR.to_s,
          'VAR_SPLITTER' => VAR_SPLITTER,
        }
      }
    ).then do |result|
      result.out.split(/\r*\n#{::Regexp.quote(VAR_SPLITTER)}\r*\n/).last
    end
  end
end

esp_variables = ::VariableImporter.new

libapplication_path = ::APPLICATION_BUILD_DIR.join('libapplication.a')
file libapplication_path => application_object_paths do |task|
  ::POSIX::Spawn.system(
    esp_variables.ar,
    'rcs',
    task.name,
    *task.prerequisites
  ).tap do |success|
    exit($?.exitstatus) unless success
  end
end

map_object_to_source = lambda do |object|
  object.sub(
    /\A#{::Regexp.quote(
      ::APPLICATION_BUILD_DIR.to_s
    )}\/*(\S*)\.o\z/,
    '\1'
  )
end

rule '.o' => [map_object_to_source] do |task|
  ::FileUtils.mkdir_p(::File.dirname(task.name))
  ::POSIX::Spawn.system(
    esp_variables.cc,
    *esp_variables.cc_params.split(/\s+/),
    "-Wa,-a,-ad,-alms=#{
      task.name
      .sub(/\.o\z/, '.lst')
      .then(&::Shellwords.method(:escape))
    }",
    *APPLICATION_INCLUDE_PATHS.map do |path|
      "-I#{path}"
    end,
    task.source,
    '-c',
    '-o',
    task.name
  ).tap do |success|
    exit($?.exitstatus) unless success
  end
end

platform_make = lambda do |task, env: {}|
  ::POSIX::Spawn.system(
    ::MAKE_COMMAND,
    '-f',
    ::PLATFORM_MAKEFILE,
    '-j4',
    '--no-print-directory',
    task,
    {
      env: {
        'BUILD_DIR_BASE' => ::ESP_BUILD_DIR.to_s,
      }.merge(env)
    }
  ).tap do |success|
    exit($?.exitstatus) unless success
  end
end

file esp_variables.output_elf => [libapplication_path.to_s] do |task|
  lib_file = task.prerequisites.first
  lib_dir = ::File.dirname(lib_file)
  lib_name = lib_file.sub(/.*\blib(\w*)\.a\z/, '\1')
  additional_flags = "-L./#{lib_dir} -l#{lib_name}"
  ::FileUtils.rm_f(task.name)
  platform_make.call('all', env: {
    'SUGGESTED_APPLICATION_LDFLAGS' => additional_flags,
  })
end

task build: esp_variables.output_elf

task flash: :build do |task|
  platform_make.call(task.name)
end

%i[menuconfig monitor].each do |task_name|
  task task_name do |task|
    platform_make.call(task.name)
  end
end
