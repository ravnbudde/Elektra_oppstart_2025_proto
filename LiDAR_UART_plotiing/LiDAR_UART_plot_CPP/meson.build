project('GUI Project', 'cpp',
  version : '0.1',
  default_options : ['warning_level=2',
                     'cpp_std=c++20',
                     'c_std=c17',
                     'default_library=static'])

sfml_include = include_directories('C:/SFML-2.5.1/include')
sfml_libpath = 'C:/SFML-2.5.1/lib'

sfml_deps = declare_dependency(
  include_directories: sfml_include,
  link_args: [
    '-L' + sfml_libpath,
    '-lsfml-graphics',
    '-lsfml-window',
    '-lsfml-system',
    # Legg til andre nødvendige SFML-moduler her
  ]
)

# Andre avhengigheter
if host_machine.system() == 'windows'
  sdl2_dep = subproject('sdl2_windows').get_variable('sdl2_windows_dep')
else
  sdl2_dep = dependency('sdl2')
endif

animationwindow_dep = dependency('animationwindow', fallback: ['animationwindow', 'animationwindow_dep'])
std_lib_facilities_dep = dependency('std_lib_facilities', fallback: ['std_lib_facilities', 'std_lib_facilities_dep'])

# Kompileringsflagg
if host_machine.system() == 'linux'
  compiler_flags = ['-Wconversion', '-fdiagnostics-color=always', '-Werror=return-type']
else
  compiler_flags = ['-Wconversion', '-fdiagnostics-color=always', '-Werror=return-type', '-fcolor-diagnostics', '-fansi-escape-codes']
endif

# Kildekoder
src = ['main.cpp']

# Eksekverbar fil
exe = executable(
  'program',
  src,
  'main.cpp',
  dependencies : [sfml_deps, animationwindow_dep, sdl2_dep, std_lib_facilities_dep],
  cpp_args : compiler_flags
)
