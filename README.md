# Pawn.Query

Pawn.Query - server side plugin that allows you to intercept and modify the samp query mechanism

---

### **What exactly does the plugin intercept**

##### The plugin can intercept queries such as

- Players List
- Server Information
- Detailed Player List
- Server Rules
- Ping

---

```c++
#include <a_samp>
#include <Pawn.Query>

main() {}

public OnGameModeInit()
{
	for (new i = 0; i < Query_HookType; i++)
	{
		EnableHookQuery(Query_HookType: i, true);
		// Hook all types
	}

	new Players[][Query_Players] = {

		{ "Dan", 15 },
		{ "Mike", 25 },
		{ "Naruto", 50 },
		{ "Sakura", 200 }
	};

	SetServerPlayers(Players); // Set a players

	//////////////////////////////////////////

	new Rules[][Query_Rules] = {

		{ "Rule 1", "Don't cheating" },
		{ "Rule 2", "Don't scam" },
		{ "Rule 3..?", "No, it enough" }
	};

	SetServerRules(Rules); // Set a rules

	//////////////////////////////////////////

	SetServerInformation(true, 228, "Shippuden", "Just gamemode", "Doesn't matter");

	//////////////////////////////////////////

	SendPing(false); // Don't send a ping

	return 1;
}
```

---

### **Download**

[Pawn.Query](https://github.com/oukibt/Pawn.Query/releases/tag/1.0)

---

[Wiki](https://github.com/oukibt/Pawn.Query/wiki)
