# C++ WinApiStockMarket
A simple 1 Server N Clients project holding three different programs, server, board and client. Mimicking a basic stock market
<p>This project uses the tools concieved by the Win32 API, therefore it shloud only run on Windows Operating Systems.
It is composed by three programs Bolsa(Server), Board and Client.

<b>Bolsa</b>:
<br>The <b>bolsa</b> program is the central component of an online stock market system. It manages all data and operations related to stock trading, including:
<ul>
    <li><strong><em>Stock Market Management</em></strong>: Controls company data, available stocks, and prices.</li>
    <li><strong><em>Trading Operations</em></strong>: Facilitates buying and selling of stocks, updates prices based on supply and demand.</li>
    <li><strong><em>User Management</em></strong>: Handles user authentication and account information.</li>
    <li><strong><em>Communication</em></strong>: Interacts with client programs via named pipes and board programs via shared memory.</li>
    <li><strong><em>Administrative Commands</em></strong>: Supports adding companies, listing companies, adjusting prices, pausing operations, and closing the system.</li>
</ul>
<br>This program ensures the integrity and functionality of the online stock market by running a single instance that can manage multiple clients concurrently.

<br><br>
<b>Board:</b>
<br>The <b>Board</b> program displays the most valuable companies in descending order and the details of the latest transaction. Key features include:
<br>
<ul>
    <li><strong><em>Company Rankings</em></strong>: Shows a list of the N most valuable companies.</li>
    <li><strong><em>Latest Transaction</em></strong>: Displays the company name, number of shares, and transaction value of the latest trade.</li>
    <li><strong><em>Automatic Updates</em></strong>: Continuously updates the displayed information.</li>
    <li><strong><em>Multiple Instances</em></strong>: Supports multiple simultaneous executions without requiring user authentication.</li>
    <li><strong><em>Communication</em></strong>: Uses shared memory for interaction with the "bolsa" program.</li>
</ul>

<br>
<b>Client:</b> <br>
The <b>Client</b> program serves as the user interface for interacting with the stock market system. Key features include:
<br>
<ul>
    <li><strong><em>User Interface</em></strong>: Provides a text-based console interface for user commands.</li>
    <li><strong><em>Authentication</em></strong>: Requires user login with a username and password.</li>
    <li><strong><em>Trading Operations</em></strong>: Allows users to buy and sell stocks and view their balance.</li>
    <li><strong><em>Real-time Interaction</em></strong>: Responds immediately to user commands and updates from the "bolsa" program.</li>
    <li><strong><em>Communication</em></strong>: Interacts with the "bolsa" program using named pipes.</li>
</ul>
