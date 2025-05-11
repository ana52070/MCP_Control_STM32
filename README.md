# MCP控制STM32

## MCP部分

### 1.下载源码

```
git clone https://github.com/ana52070/MCP_Control_STM32.git
cd MCP_Control_STM32
```



### 2. 创建并激活虚拟环境

为了避免不同项目之间的依赖冲突，建议使用虚拟环境。根据你的操作系统和 Python 版本，创建并激活虚拟环境：

#### 对于 Python 3.3 及以上版本

```bash
python -m venv .venv
```

#### 在 Windows 上激活虚拟环境

```bash
.venv\Scripts\activate
```

#### 在 Linux 或 macOS 上激活虚拟环境

```bash
source .venv/bin/activate
```



### 3. 安装依赖

直接使用`pip`安装依赖：

```bash
pip install httpx>=0.28.1 mcp>=1.8.0 openai>=1.78.0 python-dotenv>=1.1.0
```





### 4.修改代码

修改.env文件为自己的大语言模型：

```
BASE_URL=http://localhost:11434/v1/
MODEL=qwen2.5:1.5b
OPENAI_API_KEY=ollama
```



修改server-led.py中的串口配置

```
# 串口配置
SERIAL_PORT = "COM1"
BAUDRATE = 115200
TIMEOUT = 1  # 秒
```





### 5.运行MCP代码

```
python client.py server-led.py  
```







## STM32部分

```
cd MCP_Control_STM32
```

使用Keil5进行编译和烧录即可，型号为STM32F103C8T6



或者支持使用hex文件手动烧录：

```
MCP_Control_STM32\STM32_Project\Objects\Project.hex
```

