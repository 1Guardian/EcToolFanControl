#!/bin/bash

TEMPS=(45 55 65 70 75 80 85)
FAN_SPEEDS=(3000 3100 3200 3300 3500 3700 4000)
MAX_FAN_SPEED=5500

CPU=`sensors | grep "Package id 0" | cut -d + -f 2 | cut -d . -f1`
FAN_RPM=`ectool pwmgetfanrpm | cut -d : -f2`

HYSTERESIS=3
MARGIN=100

PREV_IDX=0
IDX=0

while [ $(( ${FAN_SPEEDS[${PREV_IDX}]} + ${MARGIN} )) -le ${FAN_RPM} ] && [ ${PREV_IDX} -le $(( ${#FAN_SPEEDS[@]} - 1 )) ]
	do
   	PREV_IDX=$(( ${PREV_IDX} + 1 ))
done

while [ ${TEMPS[${IDX}]} -le ${CPU} ] && [ ${IDX} -le $(( ${#TEMPS[@]} - 1 )) ]
   do
      IDX=$(( ${IDX} + 1 ))
done

if [ ${CPU} -gt $(( ${TEMPS[ $(( ${#TEMPS[@]} - 1 )) ]} )) ]; then
   ectool pwmsetfanrpm ${MAX_FAN_SPEED}
elif [ ${FAN_RPM} -lt $(( ${FAN_SPEEDS[0]} - ${MARGIN} )) ]; then
   ectool pwmsetfanrpm ${FAN_SPEEDS[${IDX}]}
else
   if [ ${IDX} -gt ${PREV_IDX} ]; then
      ectool pwmsetfanrpm ${FAN_SPEEDS[${IDX}]}
   elif [ ${IDX} -lt ${PREV_IDX} ] && [ ${CPU} -le $(( ${TEMPS[${IDX}]} - ${HYSTERESIS} )) ]; then
      ectool pwmsetfanrpm ${FAN_SPEEDS[${IDX}]}
   fi
fi


ectool pwmgetfanrpm
