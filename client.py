# 导入必要的模块
import asyncio  # 异步IO支持
import os  # 操作系统接口
import json  # JSON处理
import sys  # 系统相关功能
from typing import Optional  # 类型注解支持
from contextlib import AsyncExitStack  # 异步资源管理
from openai import OpenAI  # OpenAI客户端库
from dotenv import load_dotenv  # 环境变量加载
from mcp import ClientSession, StdioServerParameters  # MCP相关类
from mcp.client.stdio import stdio_client  # MCP标准IO客户端

# 加载.env文件中的环境变量
load_dotenv()

class MCPClient:
    """MCP客户端类，负责与OpenAI和MCP服务器交互"""
    
    def __init__(self):
        """初始化MCP客户端，设置OpenAI连接参数"""
        self.exit_stack = AsyncExitStack()  # 异步资源管理栈
        # 从环境变量读取配置
        self.openai_api_key = os.getenv("OPENAI_API_KEY")  # OpenAI API密钥
        self.base_url = os.getenv("BASE_URL")  # OpenAI API基础URL
        self.model = os.getenv("MODEL")  # 使用的模型名称
        
        if not self.openai_api_key:
            raise ValueError("❌ 未找到 OpenAI API Key，请在 .env 文件中设置 OPENAI_API_KEY")
            
        # 初始化OpenAI客户端
        self.client = OpenAI(api_key=self.openai_api_key, base_url=self.base_url)
        self.session: Optional[ClientSession] = None  # MCP会话对象

    async def connect_to_server(self, server_script_path: str):
        """
        连接到MCP服务器
        :param server_script_path: 服务器脚本路径(.py或.js)
        """
        # 检查脚本类型
        is_python = server_script_path.endswith('.py')
        is_js = server_script_path.endswith('.js')
        
        if not (is_python or is_js):
            raise ValueError("服务器脚本必须是 .py 或 .js 文件")
            
        # 根据脚本类型设置启动命令
        command = "python" if is_python else "node"
        server_params = StdioServerParameters(
            command=command,
            args=[server_script_path],
            env=None
        )
        
        # 建立标准IO连接
        stdio_transport = await self.exit_stack.enter_async_context(
            stdio_client(server_params))
        self.stdio, self.write = stdio_transport
        
        # 初始化MCP会话
        self.session = await self.exit_stack.enter_async_context(
            ClientSession(self.stdio, self.write))
        await self.session.initialize()
        
        # 获取并显示可用工具列表
        response = await self.session.list_tools()
        tools = response.tools
        print("\n已连接到服务器，支持以下工具:", [tool.name for tool in tools])

    async def process_query(self, query: str) -> str:
        """
        处理用户查询，可能调用MCP工具
        :param query: 用户输入
        :return: 处理结果
        """
        messages = [{"role": "user", "content": query}]  # 初始消息
        
        # 获取可用工具列表并转换为OpenAI工具格式
        response = await self.session.list_tools()
        available_tools = [{
            "type": "function",
            "function": {
                "name": tool.name,
                "description": tool.description,
                "input_schema": tool.inputSchema
            }
        } for tool in response.tools]
        
        # 调用OpenAI API
        response = self.client.chat.completions.create(
            model=self.model,
            messages=messages,
            tools=available_tools
        )
        
        # 处理API响应
        content = response.choices[0]
        if content.finish_reason == "tool_calls":  # 需要调用工具
            tool_call = content.message.tool_calls[0]
            tool_name = tool_call.function.name
            tool_args = json.loads(tool_call.function.arguments)
            
            # 执行工具调用
            result = await self.session.call_tool(tool_name, tool_args)
            print(f"\n\n[Calling tool {tool_name} with args {tool_args}]\n\n")
            
            # 更新消息历史
            messages.append(content.message.model_dump())
            messages.append({
                "role": "tool",
                "content": result.content[0].text,
                "tool_call_id": tool_call.id,
            })
            
            # 获取最终响应
            response = self.client.chat.completions.create(
                model=self.model,
                messages=messages,
            )
            return response.choices[0].message.content
            
        return content.message.content  # 直接返回模型响应

    async def chat_loop(self):
        """运行交互式聊天循环"""
        print("\n🤖 MCP 客户端已启动！输入 'quit' 退出")
        while True:
            try:
                query = input("\n你: ").strip()
                if query.lower() == 'quit':
                    break
                
                # 处理查询并显示结果
                response = await self.process_query(query)
                print(f"\n🤖 OpenAI: {response}")
            except Exception as e:
                print(f"\n⚠ 发生错误: {str(e)}")

    async def cleanup(self):
        """清理资源，关闭所有打开的连接"""
        await self.exit_stack.aclose()

async def main():
    """主函数"""
    if len(sys.argv) < 2:  # 检查参数
        print("Usage: python client.py <path_to_server_script>")
        sys.exit(1)
        
    client = MCPClient()
    try:
        # 连接服务器并启动聊天循环
        await client.connect_to_server(sys.argv[1])
        await client.chat_loop()
    finally:
        # 确保资源被正确释放
        await client.cleanup()

if __name__ == "__main__":
    # 启动异步主函数
    asyncio.run(main())
