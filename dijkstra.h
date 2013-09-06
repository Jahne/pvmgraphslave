int allselected(int *selected, int max) {
	int i;
	for (i = 0; i < max; i++)
		if (selected[i] == 0)
			return 0;
	return 1;
}

void shortpath(int* cost[], int max, int startVertex, int infinite, int *preced, int *distance) {

	int *selected = (int *) malloc(sizeof(int) * max);
	memset(selected, 0, sizeof(*selected) * max);

	int current = startVertex, i, k, dc, smalldist, newdist;
	for (i = 0; i < max; i++)
		distance[i] = infinite;
	selected[current] = 1;
	distance[current] = 0;

	int stopCondition = 0;

	while (!stopCondition) {
		smalldist = infinite;
		dc = distance[current];
		for (i = 0; i < max; i++) {
			if (selected[i] == 0) {
				newdist = dc + cost[current][i];
				if (newdist < distance[i]) {
					distance[i] = newdist;
					preced[i] = current;
				}
				if (distance[i] < smalldist) {
					smalldist = distance[i];
					k = i;
				}
			}
		}
		current = k;
		selected[current] = 1;

		if (!allselected(selected, max)) {
			current = (k == max - 1 ? current = 0 : k);
		} else {
			stopCondition = 1;
		}
	}
}
