local line = io.read()

-- This is not nice, it cannot work on arbitrary length streams
local str = ""
while line ~= nil do
	str = str .. line
	line = io.read()
end

function summarize_mults(str)
	local sum = 0
	local format = "mul%(%d+,%d+%)"
	for mul in string.gmatch(str, format) do
		local _, _, x, y = string.find(mul, "(%d+),(%d+)")
		sum = sum + x * y
	end
	return sum
end

str = string.gsub(str, "don't%(%).-do%(%)", "")
str = string.gsub(str, "don't%(%).*$", "")

print(summarize_mults(str))
