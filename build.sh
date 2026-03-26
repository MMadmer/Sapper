#!/usr/bin/env bash

set -euo pipefail

preset="${1:-}"
version="${2:-3.0.2}"

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
deps_root="${repo_root}/.deps"
sfml_dir="${deps_root}/SFML-${version}"

pause_if_interactive() {
    if [[ -t 0 && -t 1 ]]; then
        read -r -p "Press Enter to continue..." _
    fi
}

on_exit() {
    local status=$?

    if [[ "${status}" -eq 0 ]]; then
        echo "Build script finished."
    else
        echo "Build script failed with exit code ${status}." >&2
    fi

    pause_if_interactive
}

fail() {
    echo "${1}" >&2
    exit 1
}

is_interactive() {
    [[ -t 0 && -t 1 ]]
}

choose_preset() {
    if ! is_interactive; then
        fail "No build preset was provided. Run ./build.sh [release|debug] [sfml-version] in non-interactive shells."
    fi

    while true; do
        printf "Select build preset:\n"
        printf "  1. release\n"
        printf "  2. debug\n"
        read -r -p "Choose [1/2]: " selection

        case "${selection}" in
            1)
                preset="release"
                ;;
            2)
                preset="debug"
                ;;
            *)
                echo "Invalid choice: ${selection}" >&2
                echo >&2
                continue
                ;;
        esac

        echo "Selected preset: ${preset}"
        return 0
    done
}

validate_preset() {
    if [[ -z "${preset}" ]]; then
        choose_preset
        return 0
    fi

    if [[ "${preset}" != "debug" && "${preset}" != "release" ]]; then
        fail "Unsupported preset: ${preset}. Use: ./build.sh [release|debug] [sfml-version]"
    fi
}

ensure_compiler() {
    if [[ -n "${CXX:-}" ]]; then
        echo "Using compiler from CXX=${CXX}"
        return 0
    fi

    for compiler in c++ clang++ g++; do
        if command -v "${compiler}" >/dev/null 2>&1; then
            echo "Using compiler already available in PATH: ${compiler}"
            return 0
        fi
    done

    fail "No C++ compiler found in PATH. Install a C++17-capable toolchain or start the script with CXX set to your compiler."
}

require_tool() {
    local tool="$1"
    if ! command -v "${tool}" >/dev/null 2>&1; then
        fail "Missing required tool: ${tool}"
    fi
}

trap on_exit EXIT

validate_preset

for tool in git cmake ninja; do
    require_tool "${tool}"
done

ensure_compiler

if [[ ! -f "${sfml_dir}/CMakeLists.txt" ]]; then
    mkdir -p "${deps_root}"
    git clone --branch "${version}" --depth 1 https://github.com/SFML/SFML.git "${sfml_dir}"
fi

cd "${repo_root}"
cmake --preset "${preset}"
cmake --build --preset "${preset}"
