# Analyse et affichage des symétries

## Objectif

Ajouter un dock non modal « Symétries » qui analyse le graphe de l'onglet actif,
présente ses symétries géométriques visibles et ses automorphismes structurels,
puis les superpose au dessin sans modifier le document.

## Périmètre fonctionnel

Le dock comporte deux sections.

- **Géométriques** : réflexions axiales et rotations déduites des positions des
  nœuds. Cette détection s'inspire de `src/library/GraphSym.cpp`, avec une
  validation sur la structure du graphe.
- **Structurelles** : automorphismes de graphe, calculés même quand la mise en
  page courante n'est pas symétrique.

Un résultat sélectionné affiche son type, son ordre, le nombre de nœuds et
d'arêtes fixes, et ses orbites. Sa sélection active un calque temporaire : axe
ou centre pour une symétrie géométrique ; couleur commune par classe d'orbite
pour un automorphisme structurel. Fermer ou désélectionner le dock enlève ce
calque. Ni le graphe ni la pile d'annulation ne changent.

## Invariants d'un automorphisme

Une permutation est valide uniquement si elle préserve :

- l'adjacence des nœuds et des arêtes ;
- les types de nœuds et d'arêtes ;
- les styles visibles ;
- le nombre de brins ;
- les couleurs visibles.

Ces éléments forment des signatures initiales de sommets et d'arêtes. Des
éléments de signatures différentes ne peuvent jamais appartenir à la même
orbite.

## Algorithme structurel

Le moteur est interne, sans dépendance externe. Il procède ainsi :

1. construire les signatures visibles et une partition initiale des sommets ;
2. raffiner les partitions à partir des signatures d'incidence et des arêtes
   voisines ;
3. explorer les correspondances restantes par retour arrière, en validant les
   incidences à chaque affectation ;
4. conserver un petit ensemble de générateurs, calculer les orbites, l'ordre
   du groupe lorsque l'énumération complète reste bornée, et les statistiques
   de points/arêtes fixes.

Une limite configurable de temps et de permutations protège l'interface pour
les graphes très symétriques. Si elle est atteinte, le résultat est marqué
« partiel » et affiche les générateurs et orbites déjà validés, sans prétendre
exhaustif.

## Intégration

Le calcul est déclenché explicitement par le dock et peut être relancé après
une modification du graphe. Les données d'analyse sont séparées de `Graph` et
des objets graphiques ; le rendu du calque est lui aussi temporaire. Le dock
est raccordé au cycle de changement d'onglet du `Main_Window` comme les docks
de grille et de propriétés existants.

## Validation

Ajouter des tests CTest ciblés couvrant :

- un graphe sans automorphisme non trivial ;
- un cycle avec rotations et réflexions ;
- la rupture d'une symétrie par chacun des attributs visibles ;
- les orbites et les éléments fixes ;
- l'arrêt borné et le statut partiel.

Valider manuellement le dock avec un graphe symétrique et un graphe dont la
géométrie masque une symétrie structurelle : l'activation, la sélection et la
fermeture du calque ne doivent altérer ni le graphe, ni son état « modifié ».
