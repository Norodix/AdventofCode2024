local inspect = require("inspect")
local C = require("luacombine")
local line = io.read()

local computers = {}
while line ~= nil do
	local _, _, x, y = string.find(line, "([a-z]+)-([a-z]+)")
	if not computers[x] then
		computers[x] = {}
	end
	computers[x][#computers[x] + 1] = y
	if not computers[y] then
		computers[y] = {}
	end
	computers[y][#computers[y] + 1] = x
	line = io.read()
end

-- print(inspect(computers))

function table_has(table, value)
	for _, v in ipairs(table) do
		if value == v then
			return true
		end
	end
	return false
end

local triplets = {}

for k, v in pairs(computers) do
	-- k = aq
	-- v = {yn, vc}
	for _, v1 in ipairs(v) do
		-- v1 = yn
		for _, v2 in ipairs(computers[v1]) do
			-- computers[v1] = {aq, cg, wh, td}
			-- v2 = cg
			if table_has(computers[v2], k) then
				-- table.append(triplets, { k, v1, v2 })
				local new = { k, v1, v2 }
				table.sort(new)
				local key = table.concat(new, ",")
				triplets[key] = new
			end
		end
	end
end

-- print(inspect(triplets))

local triplets_T = {}
for k, v in pairs(triplets) do
	for _, el in ipairs(v) do
		if string.sub(el, 1, 1) == "t" then
			triplets_T[k] = v
		end
	end
end

local count = 0
for _ in pairs(triplets_T) do
	count = count + 1
end

-- print(inspect(triplets_T))
print("Number of T triplets: " .. tostring(count))

-- local f = assert(io.open("graph.dot", "w"))
-- f:write("strict graph {\n")
-- for k, v in pairs(computers) do
-- 	for _, n in ipairs(v) do
-- 		f:write(k .. " -- " .. n)
-- 		f:write("\n")
-- 	end
-- end
-- f:write("}\n")

function is_clique(comb)
	for n = 1, #comb do
		for m = n + 1, #comb do
			if not table_has(computers[comb[n]], comb[m]) then
				-- print("Table: " .. inspect(computers[comb[n]]) .. " has no " .. comb[m])
				return false
			end
		end
	end
	return true
end
-- print(is_clique({ "zo", "rp", "du" }))
-- print(is_clique({ "zo", "rp", "zp" }))

local largest_clique = {}

-- For a decreasing number of clique size, generate all combinations
-- check if the combination is a clique
for k, v in pairs(computers) do
	local found = false
	local net = { k, unpack(v) }
	-- print(inspect(net))
	for n = #net, 2, -1 do
		-- No need to check further, this wont be the largest
		if n <= #largest_clique then
			break
		end
		-- for every combination with size n
		local f = C.combn(net, n)
		while true do
			local comb = { f() }
			if #comb == 0 then
				break
			end
			if is_clique(comb) then
				table.sort(comb)
				print("Found clique")
				print(inspect(comb))
				largest_clique = { unpack(comb) }
				found = true
				break
			end
		end
	end
end

-- print largest_clique in the expected format
for i, v in ipairs(largest_clique) do
	io.write(v .. ",")
end
io.write("\n")
