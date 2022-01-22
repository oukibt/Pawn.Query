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
		EnableHookQuery(i, true);
		// Hook all types
	}

	new Players[][Query_Players] = {

		{ "Dan", 15 },
        { "Mike", 25 },
        { "Alex", 50 },
        { "John", 222 }
	};

	SetServerPlayers(Players); // Set a players

	//////////////////////////////////////////

	new Rules[][Query_Rules] = {

		{ "Rule 1", "Value 1" },
        { "Rule 2", "Value 2" },
        { "Rule 3..?", "No, it's enough" }
	};

	SetServerRules(Rules); // Set a rules

	//////////////////////////////////////////

	SetServerInformation(true, 111, "=== SERVER NAME ===", "My Drift GameMode", "En / Ru");

	//////////////////////////////////////////

	SendPing(false); // Don't send a ping

	//////////////////////////////////////////

    SetServerRule("Rule 4", "Sooo..."); // Add another rule
    RemoveServerRule("Rule 3..?"); // Remove one of the rules

	return 1;
}
```


![Result](https://cdn.discordapp.com/attachments/582915540285128725/934332530813988874/unknown.png)

---

### **Download**

[Pawn.Query](https://github.com/oukibt/Pawn.Query/releases/tag/1.1)

---

[Wiki](https://github.com/oukibt/Pawn.Query/wiki)
