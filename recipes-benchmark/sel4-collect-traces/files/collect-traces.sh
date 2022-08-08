#!/bin/sh

function usage () {
     echo "Usage: $(basename $0) [-f 'funcs to trace'] [-t sec] [-c]"
     exit 1
}

while getopts ':cf:t:h' opt; do
  case "$opt" in
    f)
      func="$OPTARG"
      ;;

    c)
      cat /sys/kernel/debug/tracing/available_filter_functions
      exit 0
      ;;

    t)
      time="$OPTARG"
      ;;

    h)
      usage
      ;;

    :)
      echo -e "option requires an argument."
      usage
      ;;

    ?)
      echo -e "Invalid command option."
      usage
      ;;
  esac
done
shift "$(($OPTIND -1))"

if [ -z "$func" ]
then
    usage
fi

if [ -z "$time" ]
then
    usage
fi

echo "start collecting data for following functions: ${func} for ${time} sec"

echo 'Stop ftrace'
echo 0 > /sys/kernel/debug/tracing/tracing_on

echo 'Set PMU as clock source'
echo pmu_clock > /sys/kernel/debug/tracing/trace_clock

echo 'Configure ftrace'
echo ${func} > /sys/kernel/debug/tracing/set_ftrace_filter
echo function > /sys/kernel/debug/tracing/current_tracer

echo 'Start ftrace'
echo 1 > /sys/kernel/debug/tracing/tracing_on

echo 'Start seL4 trace'
echo 1 > /sys/kernel/debug/sel4_tracebuffer/trace_on

echo Wait ${time}
sleep ${time}

echo 'Start seL4 trace'
echo 0 > /sys/kernel/debug/sel4_tracebuffer/trace_on

echo 'Stop ftrace'
echo 0 > /sys/kernel/debug/tracing/tracing_on

echo 'Collect ftrace logs'
trace-cmd extract

echo 'Collect seL4 trace logs'
cat /sys/kernel/debug/sel4_tracebuffer/tracedata > sel4.dat

echo 'Collect seL4 trace txt logs'
cat /sys/kernel/debug/sel4_tracebuffer/trace > sel4.txt

echo 'Convert seL4 logs to ftrace'
sel4-extract sel4.dat sel4.out > /dev/null

echo 'Merge ftrace and seL4 logs'
ftrace-merge trace.dat sel4.out > /dev/null

echo 'Remove tmp files'
rm ./sel4.dat
rm ./sel4.txt
rm ./trace.dat
rm ./sel4.out
