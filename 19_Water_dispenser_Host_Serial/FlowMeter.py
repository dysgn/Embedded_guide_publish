from threading import Event
from typing import Sequence
from collections.abc import Callable
from Dispatcher import Dispatcher
from Device import Device
from ErrorCode import ErrorCode

ReadFlowCmd		    = "RF"
ResetVolumeCmd      = "RV"
ReportEnableCmd 	= "RE"
ReportDisableCmd	= "RD"
PeriodicReportMsg   = "PR"

class FlowMeter(Device):
    def __init__(self, deviceId: str, dispatcher: Dispatcher.Dispatcher):
        super().__init__(deviceId)
        self.dispatcher = dispatcher

        self.dispatcher.addOpFormat(deviceId + ReadFlowCmd, "", "ff")
        self.dispatcher.addOpFormat(deviceId + ReportEnableCmd, "u", "")
        self.dispatcher.addOpFormat(deviceId + ReportDisableCmd, "", "")
        self.dispatcher.addOpFormat(deviceId + PeriodicReportMsg, "", "ff")

    def QueryPower(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ReadFlowCmd, callback=completeCallback, timeout=0.2)

    def ResetVolume(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ResetVolumeCmd, callback=completeCallback, timeout=0.2)

    def ReportEnable(self, interval: float, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ReportEnableCmd, (interval * 1000,), callback=completeCallback, timeout=0.2)

    def ReportDisable(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ReportDisableCmd, callback=completeCallback, timeout=0.2)
