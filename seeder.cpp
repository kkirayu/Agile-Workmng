#include "seeder.h"
#include "structures.h" // Diperlukan untuk mengakses struct dan variabel global
#include <ctime>       // Diperlukan untuk fungsi time()

void loadDummyData() {
    // --- Create Dummy Users ---
    UserNode* user1 = new UserNode{{"d.wirawan", "pass123"}, nullptr};
    UserNode* user2 = new UserNode{{"s.lestari", "pass456"}, user1};
    UserNode* user3 = new UserNode{{"b.prasetyo", "pass789"}, user2};
    headUser->next = user3; // Link after admin

    // =====================================================================
    // === Project 1: MyXL App Revamp (20 Tasks) ===
    // =====================================================================
    ProjectNode* p1 = new ProjectNode();
    p1->data = {
        "MyXL App Revamp", 1, "Complete overhaul of the MyXL customer application.",
        "XL-Axiata Tbk.", "P-MYXL-25", "Indra Setiawan", "v3.0", 1, nullptr
    };
    p1->next = nullptr;
    headProyek = p1;
    nextProjectId = 2;
    p1->data.nextTaskId = 21;

    // --- Tasks for Project 1 ---
    TaskNode* p1_t_head = nullptr;
    TaskNode* p1_t_tail = nullptr;

    auto addTaskP1 = [&](const std::string& name, Status status, const std::string& assignedTo = "") {
        TaskNode* t = new TaskNode();
        t->data = {name, status, (p1->data.nextTaskId)++, {1, 7, 2025}, {0,0,0}, assignedTo, nullptr};
        t->data.headHistory = new HistoryNode{{p1->data.version, "Task created", "admin", time(0)}, nullptr};

        if (!p1_t_head) {
            p1_t_head = p1_t_tail = t;
        } else {
            p1_t_tail->next = t;
            p1_t_tail = t;
        }
    };

    // 20 Tasks
    addTaskP1("Redesign user dashboard UI/UX", Status::ON_PROGRESS, "s.lestari");
    addTaskP1("Implement OTP login via WhatsApp", Status::ON_PROGRESS, "d.wirawan");
    addTaskP1("Develop 'Beli Paket' flow v3", Status::CLOSED, "d.wirawan");
    addTaskP1("Integrate with new payment gateway (Dana)", Status::READY); // Ready 1
    addTaskP1("Setup push notification service (Firebase)", Status::CLOSED, "b.prasetyo");
    addTaskP1("Create in-app promo banner module", Status::READY); // Ready 2
    addTaskP1("Develop customer service live chat feature", Status::NEW);
    addTaskP1("API endpoint for remaining quota", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP1("User profile & data management page", Status::NEW);
    addTaskP1("Family Plan management feature", Status::READY); // Ready 3
    addTaskP1("Optimize app startup performance", Status::ON_PROGRESS, "d.wirawan");
    addTaskP1("Setup CI/CD pipeline for Android", Status::CLOSED, "b.prasetyo");
    addTaskP1("Setup CI/CD pipeline for iOS", Status::CLOSED, "b.prasetyo");
    addTaskP1("User acceptance testing (UAT) phase 1", Status::NEW);
    addTaskP1("Publish app to Google Play Store Beta", Status::READY); // Ready 4
    addTaskP1("Publish app to Apple TestFlight", Status::READY); // Ready 5
    addTaskP1("Fix critical bug on login page", Status::ON_PROGRESS, "d.wirawan");
    addTaskP1("Security audit for user data", Status::NEW);
    addTaskP1("Onboarding tutorial for new users", Status::NEW);
    addTaskP1("Analytics integration (Firebase Analytics)", Status::CLOSED, "s.lestari");
    p1->data.headTugas = p1_t_head;

    // =====================================================================
    // === Project 2: 5G Network Rollout - Phase 1 (25 Tasks) ===
    // =====================================================================
    ProjectNode* p2 = new ProjectNode();
    p2->data = {
        "5G Network Rollout - Phase 1", 2, "Deploying 5G coverage in Jakarta & Surabaya.",
        "XL-Axiata Tbk.", "P-5G-JKSBY", "Rina Hartono", "v1.0", 1, nullptr
    };
    p2->next = p1;
    headProyek = p2;
    nextProjectId = 3;
    p2->data.nextTaskId = 26;

    TaskNode* p2_t_head = nullptr;
    TaskNode* p2_t_tail = nullptr;

    auto addTaskP2 = [&](const std::string& name, Status status, const std::string& assignedTo = "") {
        TaskNode* t = new TaskNode();
        t->data = {name, status, (p2->data.nextTaskId)++, {1, 7, 2025}, {0,0,0}, assignedTo, nullptr};
        t->data.headHistory = new HistoryNode{{p2->data.version, "Task created", "admin", time(0)}, nullptr};

        if (!p2_t_head) {
            p2_t_head = p2_t_tail = t;
        } else {
            p2_t_tail->next = t;
            p2_t_tail = t;
        }
    };

    // 25 Tasks
    addTaskP2("Site acquisition for 20 BTS towers (Jakarta)", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP2("Site acquisition for 15 BTS towers (Surabaya)", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP2("Install 5G radio equipment (JKT-01 to JKT-10)", Status::CLOSED, "d.wirawan");
    addTaskP2("Install 5G radio equipment (JKT-11 to JKT-20)", Status::ON_PROGRESS, "d.wirawan");
    addTaskP2("Core network upgrade for 5G support", Status::CLOSED, "s.lestari");
    addTaskP2("Fiber optic backbone installation (Jakarta)", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP2("Perform network performance testing (JKT-01)", Status::CLOSED, "s.lestari");
    addTaskP2("Customer migration strategy planning", Status::ON_PROGRESS, "s.lestari");
    addTaskP2("Develop 5G pricing packages", Status::ON_PROGRESS, "s.lestari");
    addTaskP2("Marketing campaign for 5G launch", Status::NEW);
    addTaskP2("Legal permit for new tower installations", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP2("Power system setup for new sites", Status::READY); // Ready 6
    addTaskP2("Transmission network configuration", Status::READY); // Ready 7
    addTaskP2("Security protocol implementation for 5G network", Status::ON_PROGRESS, "d.wirawan");
    addTaskP2("Inter-operator roaming test", Status::NEW);
    addTaskP2("Staff training for 5G equipment maintenance", Status::READY); // Ready 8
    addTaskP2("Drive test simulation in Central Jakarta", Status::NEW);
    addTaskP2("Optimize signal handover between 4G and 5G", Status::READY); // Ready 9
    addTaskP2("Radio frequency interference analysis", Status::NEW);
    addTaskP2("Dashboard monitoring for 5G traffic", Status::ON_PROGRESS, "d.wirawan");
    addTaskP2("Emergency response plan for network failure", Status::NEW);
    addTaskP2("Spectrum refarming analysis", Status::READY); // Ready 10
    addTaskP2("Vendor negotiation for new equipment", Status::CLOSED, "b.prasetyo");
    addTaskP2("Integration with existing billing system", Status::ON_PROGRESS, "s.lestari");
    addTaskP2("Final report for Phase 1 progress", Status::NEW);
    p2->data.headTugas = p2_t_head;

    // =====================================================================
    // === Project 3: Axiata Enterprise Solution (30 Tasks) ===
    // =====================================================================
    ProjectNode* p3 = new ProjectNode();
    p3->data = {
        "Axiata Enterprise Solution", 3, "B2B platform for corporate clients (IoT & Cloud).",
        "XL-Axiata Business", "P-ENT-25", "Agus Salim", "v1.0-beta", 1, nullptr
    };
    p3->next = p2;
    headProyek = p3;
    nextProjectId = 4;
    p3->data.nextTaskId = 31;

    TaskNode* p3_t_head = nullptr;
    TaskNode* p3_t_tail = nullptr;

    auto addTaskP3 = [&](const std::string& name, Status status, const std::string& assignedTo = "") {
        TaskNode* t = new TaskNode();
        t->data = {name, status, (p3->data.nextTaskId)++, {1, 7, 2025}, {0,0,0}, assignedTo, nullptr};
        t->data.headHistory = new HistoryNode{{p3->data.version, "Task created", "admin", time(0)}, nullptr};

        if (!p3_t_head) {
            p3_t_head = p3_t_tail = t;
        } else {
            p3_t_tail->next = t;
            p3_t_tail = t;
        }
    };

    // 30 Tasks
    addTaskP3("Define API for IoT device management", Status::ON_PROGRESS, "d.wirawan");
    addTaskP3("Develop corporate billing module", Status::ON_PROGRESS, "s.lestari");
    addTaskP3("Create sales dashboard for enterprise team", Status::CLOSED, "s.lestari");
    addTaskP3("Onboard pilot corporate clients (3 clients)", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP3("Security audit and penetration testing", Status::NEW);
    addTaskP3("Design multi-tenant architecture", Status::CLOSED, "d.wirawan");
    addTaskP3("Cloud infrastructure setup on AWS", Status::CLOSED, "b.prasetyo");
    addTaskP3("Develop user & role management module", Status::READY); // Ready 11
    addTaskP3("Create REST API documentation (Swagger)", Status::READY); // Ready 12
    addTaskP3("Service Level Agreement (SLA) document", Status::NEW);
    addTaskP3("Develop data analytics module for IoT data", Status::NEW);
    addTaskP3("Client-side SDK for IoT device integration", Status::NEW);
    addTaskP3("Support ticket system integration (Zendesk)", Status::READY); // Ready 13
    addTaskP3("Load testing for 1000 concurrent devices", Status::NEW);
    addTaskP3("Develop data export feature (CSV, PDF)", Status::ON_PROGRESS, "s.lestari");
    addTaskP3("Training material for sales team", Status::CLOSED, "b.prasetyo");
    addTaskP3("Training material for client onboarding", Status::NEW);
    addTaskP3("Automated deployment script (Ansible/Terraform)", Status::READY); // Ready 14
    addTaskP3("Database schema design for multi-tenancy", Status::CLOSED, "d.wirawan");
    addTaskP3("Real-time alert system for device failure", Status::NEW);
    addTaskP3("Product marketing page development", Status::ON_PROGRESS, "s.lestari");
    addTaskP3("Legal review for terms and conditions", Status::ON_PROGRESS, "b.prasetyo");
    addTaskP3("Client feedback collection form (Pilot)", Status::CLOSED, "s.lestari");
    addTaskP3("Log management setup (ELK Stack)", Status::READY); // Ready 15
    addTaskP3("Unit testing for billing module", Status::ON_PROGRESS, "d.wirawan");
    addTaskP3("Integration testing for API endpoints", Status::NEW);
    addTaskP3("Setup VPN access for corporate clients", Status::NEW);
    addTaskP3("Define disaster recovery plan", Status::NEW);
    addTaskP3("User guide and technical documentation", Status::NEW);
    addTaskP3("Final presentation to stakeholders", Status::NEW);
    p3->data.headTugas = p3_t_head;
}