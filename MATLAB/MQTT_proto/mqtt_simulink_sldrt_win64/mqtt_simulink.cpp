/*
 * mqtt_simulink.cpp
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "mqtt_simulink".
 *
 * Model version              : 1.0
 * Simulink Coder version : 24.1 (R2024a) 19-Nov-2023
 * C++ source code generated on : Wed Apr  9 19:27:34 2025
 *
 * Target selection: sldrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Linux 64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "mqtt_simulink.h"
#include <cstring>
#include "mqtt_simulink_dt.h"

/* list of Simulink Desktop Real-Time timers */
extern "C" const int SLDRTTimerCount = 1;
extern "C" const double SLDRTTimers[2] =
{
  0.2, 0.0,
};

/* Block signals (default storage) */
B_mqtt_simulink_T mqtt_simulink_B;

/* Block states (default storage) */
DW_mqtt_simulink_T mqtt_simulink_DW;

/* Real-time model */
RT_MODEL_mqtt_simulink_T mqtt_simulink_M_{ };

RT_MODEL_mqtt_simulink_T *const mqtt_simulink_M{ &mqtt_simulink_M_ };

/* Model output function */
void mqtt_simulink_output(void)
{
  /* Constant: '<Root>/Constant' */
  mqtt_simulink_B.Constant = mqtt_simulink_P.accel_x;

  /* DataStoreRead: '<Root>/Data Store Read' */
  mqtt_simulink_B.DataStoreRead = mqtt_simulink_DW.accel_x;
}

/* Model update function */
void mqtt_simulink_update(void)
{
  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++mqtt_simulink_M->Timing.clockTick0)) {
    ++mqtt_simulink_M->Timing.clockTickH0;
  }

  mqtt_simulink_M->Timing.t[0] = mqtt_simulink_M->Timing.clockTick0 *
    mqtt_simulink_M->Timing.stepSize0 + mqtt_simulink_M->Timing.clockTickH0 *
    mqtt_simulink_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void mqtt_simulink_initialize(void)
{
  /* Start for DataStoreMemory: '<Root>/Data Store Memory' */
  mqtt_simulink_DW.accel_x = mqtt_simulink_P.DataStoreMemory_InitialValue;
}

/* Model terminate function */
void mqtt_simulink_terminate(void)
{
  /* (no terminate code required) */
}

/*========================================================================*
 * Start of Classic call interface                                        *
 *========================================================================*/
extern "C" void MdlOutputs(int_T tid)
{
  mqtt_simulink_output();
  UNUSED_PARAMETER(tid);
}

extern "C" void MdlUpdate(int_T tid)
{
  mqtt_simulink_update();
  UNUSED_PARAMETER(tid);
}

extern "C" void MdlInitializeSizes(void)
{
}

extern "C" void MdlInitializeSampleTimes(void)
{
}

extern "C" void MdlInitialize(void)
{
}

extern "C" void MdlStart(void)
{
  mqtt_simulink_initialize();
}

extern "C" void MdlTerminate(void)
{
  mqtt_simulink_terminate();
}

/* Registration function */
extern "C" RT_MODEL_mqtt_simulink_T *mqtt_simulink(void)
{
  /* Registration code */

  /* Initialize timing info */
  {
    int_T *mdlTsMap { mqtt_simulink_M->Timing.sampleTimeTaskIDArray };

    mdlTsMap[0] = 0;

    /* polyspace +2 MISRA2012:D4.1 [Justified:Low] "mqtt_simulink_M points to
       static memory which is guaranteed to be non-NULL" */
    mqtt_simulink_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    mqtt_simulink_M->Timing.sampleTimes =
      (&mqtt_simulink_M->Timing.sampleTimesArray[0]);
    mqtt_simulink_M->Timing.offsetTimes =
      (&mqtt_simulink_M->Timing.offsetTimesArray[0]);

    /* task periods */
    mqtt_simulink_M->Timing.sampleTimes[0] = (0.2);

    /* task offsets */
    mqtt_simulink_M->Timing.offsetTimes[0] = (0.0);
  }

  rtmSetTPtr(mqtt_simulink_M, &mqtt_simulink_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits{ mqtt_simulink_M->Timing.sampleHitArray };

    mdlSampleHits[0] = 1;
    mqtt_simulink_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(mqtt_simulink_M, -1);
  mqtt_simulink_M->Timing.stepSize0 = 0.2;

  /* External mode info */
  mqtt_simulink_M->Sizes.checksums[0] = (1388658152U);
  mqtt_simulink_M->Sizes.checksums[1] = (1622322113U);
  mqtt_simulink_M->Sizes.checksums[2] = (1770389379U);
  mqtt_simulink_M->Sizes.checksums[3] = (1913601816U);

  {
    static const sysRanDType rtAlwaysEnabled{ SUBSYS_RAN_BC_ENABLE };

    static RTWExtModeInfo rt_ExtModeInfo;
    static const sysRanDType *systemRan[1];
    mqtt_simulink_M->extModeInfo = (&rt_ExtModeInfo);
    rteiSetSubSystemActiveVectorAddresses(&rt_ExtModeInfo, systemRan);
    systemRan[0] = &rtAlwaysEnabled;
    rteiSetModelMappingInfoPtr(mqtt_simulink_M->extModeInfo,
      &mqtt_simulink_M->SpecialInfo.mappingInfo);
    rteiSetChecksumsPtr(mqtt_simulink_M->extModeInfo,
                        mqtt_simulink_M->Sizes.checksums);
    rteiSetTPtr(mqtt_simulink_M->extModeInfo, rtmGetTPtr(mqtt_simulink_M));
  }

  mqtt_simulink_M->solverInfoPtr = (&mqtt_simulink_M->solverInfo);
  mqtt_simulink_M->Timing.stepSize = (0.2);
  rtsiSetFixedStepSize(&mqtt_simulink_M->solverInfo, 0.2);
  rtsiSetSolverMode(&mqtt_simulink_M->solverInfo, SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  mqtt_simulink_M->blockIO = ((void *) &mqtt_simulink_B);
  (void) std::memset((static_cast<void *>(&mqtt_simulink_B)), 0,
                     sizeof(B_mqtt_simulink_T));

  /* parameters */
  mqtt_simulink_M->defaultParam = ((real_T *)&mqtt_simulink_P);

  /* states (dwork) */
  mqtt_simulink_M->dwork = ((void *) &mqtt_simulink_DW);
  (void) std::memset(static_cast<void *>(&mqtt_simulink_DW), 0,
                     sizeof(DW_mqtt_simulink_T));

  /* data type transition information */
  {
    static DataTypeTransInfo dtInfo;
    mqtt_simulink_M->SpecialInfo.mappingInfo = (&dtInfo);
    dtInfo.numDataTypes = 23;
    dtInfo.dataTypeSizes = &rtDataTypeSizes[0];
    dtInfo.dataTypeNames = &rtDataTypeNames[0];

    /* Block I/O transition table */
    dtInfo.BTransTable = &rtBTransTable;

    /* Parameters transition table */
    dtInfo.PTransTable = &rtPTransTable;
  }

  /* Initialize Sizes */
  mqtt_simulink_M->Sizes.numContStates = (0);/* Number of continuous states */
  mqtt_simulink_M->Sizes.numY = (0);   /* Number of model outputs */
  mqtt_simulink_M->Sizes.numU = (0);   /* Number of model inputs */
  mqtt_simulink_M->Sizes.sysDirFeedThru = (0);/* The model is not direct feedthrough */
  mqtt_simulink_M->Sizes.numSampTimes = (1);/* Number of sample times */
  mqtt_simulink_M->Sizes.numBlocks = (5);/* Number of blocks */
  mqtt_simulink_M->Sizes.numBlockIO = (2);/* Number of block outputs */
  mqtt_simulink_M->Sizes.numBlockPrms = (2);/* Sum of parameter "widths" */
  return mqtt_simulink_M;
}

/*========================================================================*
 * End of Classic call interface                                          *
 *========================================================================*/
