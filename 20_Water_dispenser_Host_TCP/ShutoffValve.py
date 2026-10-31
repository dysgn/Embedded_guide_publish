from threading import Event
from typing import Sequence
from collections.abc import Callable
from Dispatcher import Dispatcher
from Device import Device
from ErrorCode import ErrorCode

OpenCmd	= "Op"
CloseCmd = "Cl"
QueryStateCmd = "QS"
ReportEnableCmd	= "RE"
ReportDisableCmd = "RD"
PeriodicReportMsg = "PR"

class ShutoffValve(Device):
    def __init__(self, deviceId:str, dispatcher:Dispatcher.Dispatcher) -> None:
        super().__init__(deviceId)
        self.dispatcher = dispatcher
        self.dispatcher.addOpFormat(deviceId + OpenCmd, "", "")
        self.dispatcher.addOpFormat(deviceId + CloseCmd, "", "")
        self.dispatcher.addOpFormat(deviceId + QueryStateCmd, "", "u")

    def Open(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(OpenCmd, callback=completeCallback, timeout=0.2)

    def Close(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(CloseCmd, callback=completeCallback, timeout=0.2)

    def QueryState(self, completeCallback: Callable[[ErrorCode], Sequence[int|float|str] | None] | None = None) -> tuple[ErrorCode, Sequence[int|float|str] | None] | None:
        return self.startOperation(QueryStateCmd, callback=completeCallback, timeout=0.2)


