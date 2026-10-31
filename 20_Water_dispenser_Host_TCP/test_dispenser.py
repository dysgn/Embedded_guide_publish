import sys
import threading
from typing import Sequence
import time
from ErrorCode import ErrorCode
import TCPConnect
import Dispatcher
import ShutoffValve
import Pump
import FlowMeter

global shutoffValve
global pump
global flowMeter

def main():
    global shutoffValve
    global pump
    global flowMeter
    with TCPConnect.TCPConnect("192.68.1.1", 5000) as port:
        dispatcher = Dispatcher.Dispatcher(port)
        shutoffValve = ShutoffValve.ShutoffValve("SV", dispatcher)
        pump = Pump.Pump("Pm", dispatcher)
        flowMeter = FlowMeter.FlowMeter("FM", dispatcher)
        flowMeter.setAsyncCallback(reportFlow)
        pump.setAsyncCallback(monitorPower)
        test_set_power(100, 15)
        test_set_power_timed(75, 15)
        test_set_flow_rate(6, 20)
        test_dispense_volume(250, 8)
        test_async_stop(300, 75)
        print("That's all folk")

def test_set_power(power: float, duration: float ) -> None:
    print("Testing SetPower")
    status, _ = flowMeter.ResetVolume()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't reset cumulative volume: {status.name}")

    print("Opening valve")
    status, _ = shutoffValve.Open()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't open the valve: {status.name}")

    print("Setting up flow rate reports")
    status, _ = flowMeter.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable flow reports: {status.name}")

    print(f"Setting pump to {power}%")
    status, _ = pump.SetPower(power)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't set the pump power: {status.name}")

    time.sleep(duration)
    print("Turning pump off")
    status, _ = pump.SetPower(0)
    if status != Dispatcher.ErrorCode.SUCCESS:
        sys.exit(f"Can't turn the pump off: {status.name}")
    time.sleep(3)
    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")

def test_set_power_timed(power: float, duration: float) -> None:
    print("Testing SetTimedPower")
    status, _ = flowMeter.ResetVolume()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't reset cumulative volume: {status.name}")

    print("Opening valve")
    status, _ = shutoffValve.Open()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't open the valve: {status.name}")

    print("Setting up flow rate reports")
    status, _ = flowMeter.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable flow reports: {status.name}")

    print(f"Setting pump to {power}%")
    status, _ = pump.SetTimedPower(power, duration)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't set the pump power: {status.name}")

    time.sleep(3)
    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")

def test_set_flow_rate(rate: float, duration: float) -> None:
    print("Testing SetFlowRate")
    status, _ = flowMeter.ResetVolume()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't reset cumulative volume: {status.name}")

    print("Opening valve")
    status, _ = shutoffValve.Open()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't open the valve: {status.name}")

    print("Setting up flow rate reports")
    status, _ = flowMeter.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable flow reports: {status.name}")

    status, _ = pump.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable pump power reports: {status.name}")

    print(f"Setting flow rate to {rate} mL/s")
    status, _ = pump.SetFlowRate(rate)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't set the flow rate: {status.name}")

    time.sleep(duration)
    print("Stopping the pump")
    status, _ = pump.Stop()
    if status != Dispatcher.ErrorCode.SUCCESS:
        sys.exit(f"Can't stop the pump: {status.name}")
    time.sleep(3)
    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")

    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")
    status, _ = pump.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable pump reports: {status.name}")

def test_dispense_volume(volume: float, rate: float) -> None:
    print("Testing DispenseVolume")
    status, _ = flowMeter.ResetVolume()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't reset cumulative volume: {status.name}")

    print("Opening valve")
    status, _ = shutoffValve.Open()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't open the valve: {status.name}")

    print("Setting up flow rate reports")
    status, _ = flowMeter.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable flow reports: {status.name}")

    status, _ = pump.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable pump power reports: {status.name}")

    print (f"Dispensing {volume} mL")
    status, retData = pump.DispenseVolume(volume, rate)
    if status == ErrorCode.SUCCESS:
        print(f"{retData[0]} dispensed")
    else:
        sys.exit(f"Can't dispense the volume: {status.name}")

    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")
    status, _ = pump.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable pump reports: {status.name}")

global volumeReached
global targetVolume

def test_async_stop(volume: float, powerLevel: float, timeout: float = 30) -> None:
    print("Testing async Stop")
    global targetVolume
    targetVolume = volume
    status, _ = flowMeter.ResetVolume()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't reset cumulative volume: {status.name}")

    global volumeReached
    volumeReached = threading.Event()
    flowMeter.setAsyncCallback(monitorFlow)
    print("Opening valve")
    status, _ = shutoffValve.Open()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't open the valve: {status.name}")

    print("Setting up flow rate reports")
    status, _ = flowMeter.ReportEnable(0.5)
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't enable flow reports: {status.name}")

    print(f"Setting pump to {powerLevel}%")
    status, _ = pump.SetTimedPower(powerLevel, timeout, pumpFinished)
    if status != ErrorCode.SUCCESS and status != ErrorCode.PENDING:
        sys.exit(f"Can't set the pump power: {status.name}")
    if volumeReached.wait(timeout):
        print("Delivered")
    else:
        print("Not delivered, timeout")

    flowMeter.setAsyncCallback(reportFlow)
    volumeReached = None

    print("Closing valve")
    status, _ = shutoffValve.Close()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't close the valve: {status.name}")
    status, _ = flowMeter.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable flow reports: {status.name}")
    status, _ = pump.ReportDisable()
    if status != ErrorCode.SUCCESS:
        sys.exit(f"Can't disable pump reports: {status.name}")

def reportFlow(dev: str, opcode: str, status: int, arg: Sequence[float]) -> None:
    print(f"{arg[0]} mL/s, total {arg[1]} mL")

def monitorFlow(dev: str, opcode: str, status: int, arg: Sequence[float]) -> None:
    print(f"{arg[0]} mL/s, total {arg[1]} mL")
    global targetVolume
    if arg[1] >= targetVolume:
        print("Target reached, stopping the pump")
        pump.Stop(lambda status, retData: None)

def pumpFinished(status: ErrorCode, retData: Sequence[float| str] | None) -> None:
    print(f"Pump finished, status: {status.name}")
    volumeReached.set()

def monitorPower(dev: str, opcode: str, status: int, arg: Sequence[float]) -> None:
    print(f"Power {arg[0]}%")

if __name__ == "__main__":
    main()