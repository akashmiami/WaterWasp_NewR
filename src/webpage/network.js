const gel = (e) => document.getElementById(e);
const wifi_div = gel("wifi");
const connect_div = gel("connect");
const connect_manual_div = gel("connect_manual");
const connect_wait_div = gel("connect-wait");
const connect_details_div = gel("connect-details");

function docReady(fn) {
	if (
		document.readyState === "complete" ||
		document.readyState === "interactive"
	) {
		setTimeout(fn, 1);
	} else {
		document.addEventListener("DOMContentLoaded", fn);
	}
}

var selectedSSID = "";
var refreshAPInterval = null;
var checkStatusInterval = null;
var count_refreshAP = 2;
var count_refreshSTS = 2;
var count_connect = 3;
var url_connect = "";

function stopCheckStatusInterval() {
	if (checkStatusInterval != null) {
		clearInterval(checkStatusInterval);
		checkStatusInterval = null;
	}
}

function stopRefreshAPInterval() {
	if (refreshAPInterval != null) {
		clearInterval(refreshAPInterval);
		refreshAPInterval = null;
	}
}

function startCheckStatusInterval() {
	checkStatus();
	checkStatusInterval = setInterval(checkStatus, 3000);
}

function startRefreshAPInterval() {
	refreshAP();
	refreshAPInterval = setInterval(refreshAP, 10000);
}

docReady(async function () {
	gel("wifi-status").addEventListener(
		"click",
		() => {
			wifi_div.style.display = "none";
			document.getElementById("connect-details").style.display = "block";
		},
		false
	);

	gel("manual_add").addEventListener(
		"click",
		(e) => {
			selectedSSID = e.target.innerText;

			gel("ssid-pwd").textContent = selectedSSID;
			wifi_div.style.display = "none";
			connect_manual_div.style.display = "block";
			connect_div.style.display = "none";

			gel("connect-success").display = "none";
			gel("connect-fail").display = "none";
		},
		false
	);

	gel("wifi-list").addEventListener(
		"click",
		(e) => {
			selectedSSID = e.target.innerText;
			gel("ssid-pwd").textContent = selectedSSID;
			connect_div.style.display = "block";
			wifi_div.style.display = "none";
		},
		false
	);

	function cancel() {
		selectedSSID = "";
		connect_div.style.display = "none";
		connect_manual_div.style.display = "none";
		wifi_div.style.display = "block";
	}

	gel("cancel").addEventListener("click", cancel, false);

	gel("manual_cancel").addEventListener("click", cancel, false);

	gel("join").addEventListener("click", performConnect, false);

	gel("manual_join").addEventListener(
		"click",
		(e) => {
			performConnect("manual");
		},
		false
	);

	gel("ok-details").addEventListener(
		"click",
		() => {
			connect_details_div.style.display = "none";
			wifi_div.style.display = "block";
		},
		false
	);

	gel("ok-connect").addEventListener(
		"click",
		() => {
			connect_wait_div.style.display = "none";
			wifi_div.style.display = "block";
		},
		false
	);

	gel("disconnect").addEventListener(
		"click",
		() => {
			gel("diag-disconnect").style.display = "block";
			gel("connect-details-wrap").classList.add("blur");
		},
		false
	);

	gel("no-disconnect").addEventListener(
		"click",
		() => {
			gel("diag-disconnect").style.display = "none";
			gel("connect-details-wrap").classList.remove("blur");
		},
		false
	);

	gel("yes-disconnect").addEventListener("click", async () => {
		stopCheckStatusInterval();
		selectedSSID = "";

		document.getElementById("diag-disconnect").style.display = "none";
		gel("connect-details-wrap").classList.remove("blur");

		await fetch("disconnect", {
			method: "GET",
			headers: {
				"Content-Type": "application/json",
			}
		});

		startCheckStatusInterval();

		connect_details_div.style.display = "none";
		wifi_div.style.display = "block";
	});

	await refreshAP();
	startCheckStatusInterval();
	startRefreshAPInterval();
});

async function performConnect(conntype) {
	stopCheckStatusInterval();

	stopRefreshAPInterval();

	let pwd;
	let tz;
	let email;
	if (conntype == "manual") {
		selectedSSID = gel("manual_ssid").value;
		pwd = gel("manual_pwd").value;
		tz = gel("manual_timezone").value.replace("/", "_");
		email = gel("manual_email").value;
	} else {
		pwd = gel("pwd").value;
		tz = gel("timezone").value.replace("/", "_");
		email = gel("email").value;
	}

	gel("loading").style.display = "block";
	gel("connect-success").style.display = "none";
	gel("connect-fail").style.display = "none";

	gel("ok-connect").disabled = true;
	gel("ssid-wait").textContent = selectedSSID;
	connect_div.style.display = "none";
	connect_manual_div.style.display = "none";
	connect_wait_div.style.display = "block";

	count_connect = 0;
    let tmpstr = `ssid=${selectedSSID}&pwd=${pwd}&email=${email}&tz=${tz}`
	url_connect = `connect/`
	for (let i = 0; i < tmpstr.length; i++) {
		d = tmpstr.charCodeAt(i);
		url_connect += ("0"+(Number(d).toString(16))).slice(-2).toUpperCase()
	}
	await fetch(url_connect, {
		method: "GET",
		headers: {
			"Content-Type": "application/json"
		}
	});

	startCheckStatusInterval();
	startRefreshAPInterval();
}

function rssiToIcon(rssi) {
	if (rssi >= -60) {
		return "w0";
	} else if (rssi >= -67) {
		return "w1";
	} else if (rssi >= -75) {
		return "w2";
	} else {
		return "w3";
	}
}

async function refreshAP(url = "ap.json") {
	if (gel("network_page").style.display == "none" && count_refreshAP > 1) {
		return;
	}

	count_refreshAP += 1;

	try {
		var res = await fetch(url);
		var access_points = await res.json();
		if (access_points.length > 0) {
			access_points.sort((a, b) => {
				var x = a["rssi"];
				var y = b["rssi"];
				return x < y ? 1 : x > y ? -1 : 0;
			});
			refreshAPHTML(access_points);
		}
	} catch (e) {
		console.info("Access points returned empty from /ap.json!");
	}
}

function refreshAPHTML(data) {
	var h = "";
	data.forEach(function (e, idx, array) {
		let ap_class = idx === array.length - 1 ? "" : " brdb";
		let rssicon = rssiToIcon(e.rssi);
		let auth = e.auth == 0 ? "" : "pw";
		h += `<div class="ape${ap_class}"><div class="${rssicon}"><div class="${auth}">${e.ssid}</div></div></div>\n`;
	});

	gel("wifi-list").innerHTML = h;
}

async function checkStatus(url = "status.json") {
	if (gel("network_page").style.display == "none" && count_refreshSTS > 1) {
		return;
	}

	count_refreshSTS += 1;

	try {
		var response = await fetch(url);
		var data = await response.json();
		if (data && data.hasOwnProperty("ssid") && data["ssid"] != "") {
			if (data["ssid"] === selectedSSID) {
				switch (data["urc"]) {
					case 0:
						count_connect = 3;
						url_connect = "";
						console.info("Got connection!");
						document.querySelector(
							"#connected-to div div div span"
						).textContent = data["ssid"];
						document.querySelector("#connect-details h1").textContent =
							data["ssid"];
						gel("ip").textContent = data["ip"];
						gel("netmask").textContent = data["netmask"];
						gel("gw").textContent = data["gw"];
						gel("wifi-status").style.display = "block";
						gel("ok-connect").disabled = false;
						gel("loading").style.display = "none";
						gel("connect-success").style.display = "block";
						gel("connect-fail").style.display = "none";
						gel("connect-info").style.display = "block";
						gel("connect-info").innerHTML = `
						<p>Success!</p>
						<p>Device will reboot completed in 5s</p>
						<p>Pls connect to AP ${data["ssid"]} and access the webpage by URL</p>
						<p><a href="http://${data["ip"]}">http://${data["ip"]}</a></p>`

						await fetch("/reboot", {
							method: "GET",
							headers: {
								"Content-Type": "application/json"
							}
						});
						break;
					case 1:
						console.info("Connection attempt failed!");
						document.querySelector(
							"#connected-to div div div span"
						).textContent = data["ssid"];
						document.querySelector("#connect-details h1").textContent =
							data["ssid"];
						gel("ip").textContent = "0.0.0.0";
						gel("netmask").textContent = "0.0.0.0";
						gel("gw").textContent = "0.0.0.0";
						gel("wifi-status").style.display = "none";
						gel("ok-connect").disabled = false;
						gel("loading").style.display = "none";
						gel("connect-fail").style.display = "block";
						gel("connect-success").style.display = "none";
						break;
				}
			} else if (data.hasOwnProperty("urc") && data["urc"] === 0) {
				console.info("Connection established");
				if (
					gel("wifi-status").style.display == "" ||
					gel("wifi-status").style.display == "none"
				) {
					document.querySelector("#connected-to div div div span").textContent =
						data["ssid"];
					document.querySelector("#connect-details h1").textContent =
						data["ssid"];
					gel("ip").textContent = data["ip"];
					gel("netmask").textContent = data["netmask"];
					gel("gw").textContent = data["gw"];
					gel("wifi-status").style.display = "block";
				}
			}
		} else if (data.hasOwnProperty("urc") && data["urc"] === 2) {
			console.log("Manual disconnect requested...");
			if (gel("wifi-status").style.display == "block") {
				gel("wifi-status").style.display = "none";
			}
		}
	} catch (e) {
		console.info("Was not able to fetch /status.json");
	}

	if (count_connect < 2 && url_connect.length > 5) {
		console.log("Retry...");
		count_connect += 1;
		await fetch(url_connect, {
			method: "GET",
			headers: {
				"Content-Type": "application/json"
			}
		});
	}
}
