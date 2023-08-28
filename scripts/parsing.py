import sys
import re
import os

msg = "Parsing db_result file..."
file = sys.argv[1]

print(msg)

# 파일을 읽기 모드로 열고 내용을 가져옴
with open(file, 'r') as rf:
    content = rf.read()

# 원하는 정보 추출
memtable_stall_time_match = re.search(r'memtable stall time: (\d+)', content)
L0_stall_time_match = re.search(r'L0 stall time: (\d+)', content)
L0_slow_stall_time_match = re.search(r'L0 slow stall time: (\d+)', content)

if not memtable_stall_time_match or not L0_stall_time_match or not L0_slow_stall_time_match:
    print("Error: Could not extract required information from the file.")
    sys.exit(1)

memtable_stall_time = memtable_stall_time_match.group(1)
L0_stall_time = L0_stall_time_match.group(1)
L0_slow_stall_time = L0_slow_stall_time_match.group(1)

# 추출한 정보를 기존 내용에 추가
content += f"\nExtracted Information:\n"
content += f"memtable stall time: {memtable_stall_time}\n"
content += f"L0 stall time: {L0_stall_time}\n"
content += f"L0 slow stall time: {L0_slow_stall_time}\n"

# 변경된 내용을 새로운 파일에 쓴다.
output_file = os.path.splitext(file)[0] + "_parsed.txt"
with open(output_file, 'w') as wf:
    wf.write(content)