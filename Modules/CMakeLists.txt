# CMakeLists.txt for Panelz2/Modules

add_executable(panelz2 panelz2.cxx)

target_link_libraries(panelz2
    PRIVATE
        Solver
        absl::flags
        absl::flags_parse
        absl::flags_usage
)
