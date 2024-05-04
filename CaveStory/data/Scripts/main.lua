function ModCS.Game.Act()
	if ModCS.Key.Arms() then
		ModCS.Trigger.Create(2, 1, 0, 0, 12, 6, 8, 9, 8, 8, true, false, false) 
	end
end

ModCS.Trigger.Act[2] = function()
	print("This will always run when touching trigger 2 in any room")
end