#!/usr/bin/env zsh

set -e

ITERATIONS=${1:-3}
BUILD_DIR_BASE="build_bench"

GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Build a branch, return path to executable
build_branch() {
    local branch=$1
    local build_dir="${BUILD_DIR_BASE}_${branch}"

    # Logging goes to stderr so it is not captured
    echo "${BLUE}Building branch: ${branch}${NC}" >&2
    git checkout "$branch" > /dev/null

    rm -rf "$build_dir"
    mkdir -p "$build_dir"
    cd "$build_dir"
    cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null
    make -j$(sysctl -n hw.ncpu) > /dev/null
    cd - > /dev/null

    if [[ ! -x "${build_dir}/raytracer" ]]; then
        echo "${RED}Build failed for branch ${branch}${NC}" >&2
        exit 1
    fi

    # This is the only output to stdout, so it gets captured
    echo "${build_dir}/raytracer"
}

# Run executable and return real time in seconds 
run_benchmark() {
    local exe=$1
    local start=$(perl -MTime::HiRes -e 'print Time::HiRes::time')
    $exe > /dev/null
    local end=$(perl -MTime::HiRes -e 'print Time::HiRes::time')
    local elapsed=$(perl -e "print $end - $start")
    echo "$elapsed"
}

# --- Main ---
if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "${RED}Not in a git repository.${NC}" >&2
    exit 1
fi

# Stash changes if any
if ! git diff --quiet; then
    echo "${RED}Uncommitted changes. Stash? (y/n)${NC}" >&2
    read -q ans
    if [[ $ans == "y" ]]; then
        git stash push -m "benchmark stash" > /dev/null
        stashed=true
    else
        exit 1
    fi
fi

original_branch=$(git branch --show-current)

# Build both branches (only the path is captured)
main_exe=$(build_branch "main")
parallel_exe=$(build_branch "$original_branch")

main_times=()
parallel_times=()

echo "${BLUE}Running benchmarks with ${ITERATIONS} iterations...${NC}" >&2

for i in $(seq 1 $ITERATIONS); do
    echo "Iteration $i / $ITERATIONS" >&2

    echo -n "  main: " >&2
    main_time=$(run_benchmark "$main_exe")
    main_times+=($main_time)
    printf "${GREEN}%.2f${NC}s\n" $main_time >&2

    echo -n "  parallel: " >&2
    parallel_time=$(run_benchmark "$parallel_exe")
    parallel_times+=($parallel_time)
    printf "${GREEN}%.2f${NC}s\n" $parallel_time >&2
    echo "" >&2
done

# Restore branch and pop stash
git checkout "$original_branch" > /dev/null
if [[ -n $stashed ]]; then
    git stash pop > /dev/null
fi

# Compute averages
sum_main=0
for t in $main_times; do sum_main=$(echo "$sum_main + $t" | bc -l); done
avg_main=$(echo "$sum_main / $ITERATIONS" | bc -l)

sum_parallel=0
for t in $parallel_times; do sum_parallel=$(echo "$sum_parallel + $t" | bc -l); done
avg_parallel=$(echo "$sum_parallel / $ITERATIONS" | bc -l)

# Speedup
if (( $(echo "$avg_parallel > 0" | bc -l) )); then
    speedup=$(echo "$avg_main / $avg_parallel" | bc -l)
else
    speedup="inf"
fi

printf "${BLUE}===== Results =====${NC}\n"
printf "Main average:      ${GREEN}%.2f${NC}s\n" $avg_main
printf "Parallel average:  ${GREEN}%.2f${NC}s\n" $avg_parallel
printf "Speedup:           ${GREEN}%.2f${NC}x\n" $speedup
echo "Individual times:"
echo "  main:      ${main_times[@]}"
echo "  parallel:  ${parallel_times[@]}"