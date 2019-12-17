
"""
Create UART2 and UART3 files based on UART1 prototype. This allows making changes easily
and follow the DRY principle.
"""


from copy import deepcopy


def replace_uart(uart_id, contents: str):
  if uart_id == 2:
    contents = contents.replace('UART1',  'UART2')
    contents = contents.replace('USART1', 'USART2')
    contents = contents.replace('uart1',  'uart2')
    contents = contents.replace('usart1', 'usart2')
  elif uart_id == 3:
    contents = contents.replace('UART1',  'UART3')
    contents = contents.replace('USART1', 'USART3')
    contents = contents.replace('uart1',  'uart3')
    contents = contents.replace('usart1', 'usart3')
  return contents


def CreateUartFiles():
  print(f"Creating UART2 and 3 files from UART1 prototype...")

  # create .h files
  with open('./uart1.h', encoding='utf-8') as f:
    contents = f.read()
    uart2_contents = deepcopy(contents);
    uart2_contents = replace_uart(2, uart2_contents)
    with open('./uart2.h', 'w') as out:
      out.write(uart2_contents)
    uart3_contents = deepcopy(contents);
    uart3_contents = replace_uart(3, uart3_contents)
    with open('./uart3.h', 'w') as out:
      out.write(uart3_contents)

  print("Done.")


if __name__ == "__main__":
    CreateUartFiles()
