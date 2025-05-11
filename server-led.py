import serial
from typing import Any
from mcp.server.fastmcp import FastMCP

# 初始化 MCP 服务器
mcp = FastMCP("LEDServer")

# 串口配置
SERIAL_PORT = "COM1"
BAUDRATE = 115200
TIMEOUT = 1  # 秒

def initialize_serial() -> serial.Serial:
    """初始化串口连接"""
    try:
        ser = serial.Serial(
            port=SERIAL_PORT,
            baudrate=BAUDRATE,
            timeout=TIMEOUT
        )
        return ser
    except serial.SerialException as e:
        raise RuntimeError(f"无法打开串口 {SERIAL_PORT}: {str(e)}")

def send_serial_data(data: str) -> str:
    """
    通过串口发送ASCII字符串数据
    :param data: 要发送的ASCII字符串
    :return: 发送状态信息
    """
    try:
        ser = initialize_serial()
        print(f"正在发送串口命令: {data}")  # 调试输出
        ser.write(data.encode('ascii'))
        ser.flush()  # 确保数据发送完成
        return f"成功发送数据: {data}"
    except RuntimeError as e:
        return f"串口错误: {str(e)}"
    except Exception as e:
        return f"发送失败: {str(e)}"
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

@mcp.tool()
async def control_led(state: bool) -> str:
    """
    控制灯的开关 - 直接说"开灯"或"关灯"即可
    示例: 
    - "请开灯" -> control_led(True)
    - "把灯关掉" -> control_led(False)
    :param state: 开灯(True)或关灯(False)
    :return: 操作结果
    """
    command = "led_on\r\n" if state else "led_off\r\n"
    return send_serial_data(command)

@mcp.tool()
async def OLED_ShowString(state: str) -> str:
    """
    控制屏幕的显示内容
    :param state: 显示的内容
    :return: 操作结果
    """
    command = f"{state}\r\n"
    return send_serial_data(command)

if __name__ == "__main__":
    # 以标准 I/O 方式运行 MCP 服务器
    mcp.run(transport='stdio')
