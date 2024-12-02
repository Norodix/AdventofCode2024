local line = io.read()

function sign(number)
	return number > 0 and 1 or (number == 0 and 0 or -1)
end

function check_safe(nums)
	-- iterate through the number pairs
	local safe = true
	local firstsign = sign(nums[2] - nums[1])
	for i, _ in ipairs(nums) do
		if i == #nums then
			break
		end
		-- check if the signs match all the way through
		local d = nums[i + 1] - nums[i]
		if sign(d) ~= firstsign then
			safe = false
			print("Unsafe because the signs dont match")
			break
		end
		-- check if the diffs are not too big <= 3
		if math.abs(d) > 3 then
			safe = false
			print("Unsafe because the diffs are too big")
			break
		end
		-- check if diff is 0
		if d == 0 then
			safe = false
			print("Unsafe because the diff is 0")
			break
		end
	end
	return safe
end

local safes = 0
while line ~= nil do
	local nums = {}
	for n in string.gmatch(line, "(%d+)") do
		nums[#nums + 1] = tonumber(n)
	end
	if check_safe(nums) then
		safes = safes + 1
	end
	line = io.read()
end
print(safes)
