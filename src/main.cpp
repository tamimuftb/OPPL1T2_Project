#include <iostream>
#include <memory>
#include <string>
#include "PlaybookEngine.h"
#include "LocalExecutor.h"
#include "RemoteSSHExecutor.h"
#include "HtmlReporter.h"

static void banner()
{
    std::cout << "\033[1;31m"
        "  ███████╗███████╗███╗   ██╗████████╗██╗███╗   ██╗███████╗██╗      \n"
        "  ██╔════╝██╔════╝████╗  ██║╚══██╔══╝██║████╗  ██║██╔════╝██║      \n"
        "  ███████╗█████╗  ██╔██╗ ██║   ██║   ██║██╔██╗ ██║█████╗  ██║      \n"
        "  ╚════██║██╔══╝  ██║╚██╗██║   ██║   ██║██║╚██╗██║██╔══╝  ██║      \n"
        "  ███████║███████╗██║ ╚████║   ██║   ██║██║ ╚████║███████╗███████╗ \n"
        "  ╚══════╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝ \n"
        "\033[0m"
        "\033[1;31m  ██████╗ ███████╗██████╗ \033[0m\n"
        "\033[1;31m  ██╔══██╗██╔════╝██╔══██╗\033[0m\n"
        "\033[1;31m  ██████╔╝█████╗  ██║  ██║\033[0m"
        "  \033[1;37mAutomated TTP Emulation Engine\033[0m\n"
        "\033[1;31m  ██╔══██╗██╔══╝  ██║  ██║\033[0m"
        "  \033[0;90mFor authorized security testing only\033[0m\n"
        "\033[1;31m  ██║  ██║███████╗██████╔╝\033[0m\n"
        "\033[1;31m  ╚═╝  ╚═╝╚══════╝╚═════╝ \033[0m\n\n";
}

static void usage(const char* prog)
{
    std::cout
        << "Usage:\n"
        << "  Local  : " << prog << " <playbook.yaml> [report.html]\n"
        << "  Remote : " << prog << " <playbook.yaml> <host> <user> <pass> [port] [report.html]\n\n"
        << "Examples:\n"
        << "  " << prog << " playbooks/linux_recon.yaml\n"
        << "  " << prog << " playbooks/linux_recon.yaml report.html\n"
        << "  " << prog << " playbooks/linux_recon.yaml 192.168.56.101 ubuntu password\n"
        << "  " << prog << " playbooks/windows_recon.yaml 192.168.56.102 Administrator pass 22 win.html\n\n";
}

int main(int argc, char* argv[])
{
    banner();

    if (argc < 2) { usage(argv[0]); return 1; }

    std::string playbook   = argv[1];
    std::string reportPath = "sentinel_report.html";
    std::string host, user, pass, targetHost;
    int port = 22;

    std::unique_ptr<IExecutor> executor;

    if (argc >= 5) {
        // Remote mode
        host = argv[2]; user = argv[3]; pass = argv[4];
        targetHost = host;
        if (argc >= 6) {
            try { port = std::stoi(argv[5]); }
            catch (...) { reportPath = argv[5]; }
        }
        if (argc >= 7) reportPath = argv[6];

        std::cout << "\033[1;36m[*]\033[0m Mode   : Remote SSH\n"
                  << "\033[1;36m[*]\033[0m Target : " << host << ":" << port << "\n"
                  << "\033[1;36m[*]\033[0m User   : " << user << "\n";

        executor = std::make_unique<RemoteSSHExecutor>(host, user, pass, port);
    } else {
        // Local mode
        if (argc >= 3) reportPath = argv[2];
        std::cout << "\033[1;36m[*]\033[0m Mode   : Local\n";
        executor = std::make_unique<LocalExecutor>();
    }

    std::cout << "\033[1;36m[*]\033[0m Playbook: " << playbook << "\n"
              << "\033[1;36m[*]\033[0m Report  : " << reportPath << "\n";

    PlaybookEngine engine(std::move(executor));

    if (!engine.loadPlaybook(playbook)) {
        std::cerr << "\033[1;31m[!]\033[0m Failed to load playbook\n";
        return 1;
    }

    // Pass metadata to reporter after loadPlaybook so we have target_os
    engine.setReporter(std::make_unique<HtmlReporter>(reportPath, "", "", targetHost));
    engine.executeCampaign();

    return 0;
}
