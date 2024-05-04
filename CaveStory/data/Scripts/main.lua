function ModCS.Game.Act()
	if ModCS.Key.Arms() then
		ModCS.Trigger.Create(2, 1, 0, 12, 6, 8, 9, 8, 8, true, false, false) 
	end
end

ModCS.Trigger.Act[13][1] = function()
	ModCS.Sound.Play(17)
end