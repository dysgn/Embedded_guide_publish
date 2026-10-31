from threading import Event
from typing import Sequence
from collections.abc import Callable
from Dispatcher import Dispatcher
from Device import Device
from ErrorCode import ErrorCode

SetPowerCmd		    = "Pc"
SetTimedPowerCmd    = "Pt"
SetFlowRateCmd	    = "FR"
DispenseVolumeCmd   = "Vm"
StopCmd			    = "St"
QueryPowerCmd	    = "QP"
ReportEnableCmd	    = "RE"
ReportDisableCmd	= "RD"
PeriodicReportMsg   = "PR"

class Pump(Device):
    def __init__(self, deviceId: str, dispatcher: Dispatcher.Dispatcher):
        super().__init__(deviceId)
        self.dispatcher = dispatcher

        self.dispatcher.addOpFormat(deviceId + SetPowerCmd, "f", "")
        self.dispatcher.addOpFormat(deviceId + SetTimedPowerCmd, "fu", "")
        self.dispatcher.addOpFormat(deviceId + SetFlowRateCmd, "f", "f")
        self.dispatcher.addOpFormat(deviceId + DispenseVolumeCmd, "ff","f")
        self.dispatcher.addOpFormat(deviceId + StopCmd, "", "")
        self.dispatcher.addOpFormat(deviceId + QueryPowerCmd, "", "f")
        self.dispatcher.addOpFormat(deviceId + ReportEnableCmd, "u", "")
        self.dispatcher.addOpFormat(deviceId + ReportDisableCmd, "", "")
        self.dispatcher.addOpFormat(deviceId + PeriodicReportMsg, "", "f")

    def SetPower(self, newPower : float, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(SetPowerCmd, (newPower,), callback=completeCallback, timeout=0.2)

    def SetTimedPower(self, newPower : float, seconds: float,
                      completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(SetTimedPowerCmd, (newPower, seconds * 1000), callback=completeCallback,
                                   timeout=seconds + 0.2)

    def SetFlowRate(self, newFlowRate: float, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(SetFlowRateCmd, (newFlowRate,), callback=completeCallback, timeout=30)

    def DispenseVolume(self, volume: float, flowRate:float,
                       completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(DispenseVolumeCmd, (volume, flowRate,), callback=completeCallback, timeout=volume / flowRate * 1.5)

    def Stop(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(StopCmd, callback=completeCallback, timeout=0.2)

    def QueryPower(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(QueryPowerCmd, callback=completeCallback, timeout=0.2)

    def ReportEnable(self, interval: float, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ReportEnableCmd, (interval * 1000,), callback=completeCallback, timeout=0.2)

    def ReportDisable(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(ReportDisableCmd, callback=completeCallback, timeout=0.2)
