# GitLab Wiki Upload

## Option 1: Direkt im GitLab Web-UI

1. In GitLab Projekt auf `Wiki` gehen.
2. Seite `home` anlegen und Inhalt aus `wiki/home.md` einfuegen.
3. Fuer jede weitere Datei unter `wiki/` eine Seite mit gleichem Namen anlegen:
   - `Installation-und-Start`
   - `Spielanleitung`
   - `Level-Editor`
   - `Technische-Architektur`
   - `Dateiformat-Level`
4. Optional `_sidebar.md` als Sidebar-Seite einpflegen.

## Option 2: Wiki als Git-Repo pushen

```bash
git clone <GITLAB_PROJECT_URL>.wiki.git
cd <project>.wiki
cp /Pfad/zu/project1/wiki/*.md .
git add .
git commit -m "Add project wiki"
git push
```

Hinweis: Dateiname `home.md` ist die Startseite.

