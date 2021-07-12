<template>
  <v-content>
    <v-card
        class="mx-auto text-center ma-2"
        color="green"
        dark
        max-width="600"
        v-for="diagram of resultsArray"
        :key="diagram.name"
    >
      <v-card-text>
        <v-sheet color="rgba(0, 0, 0, .12)">
          <v-sparkline
              :value="diagram.vals"
              color="rgba(255, 255, 255, .7)"
              height="100"
              padding="24"
              stroke-linecap="round"
              smooth
          >
            <template v-slot:label="item">
              {{ item.value }}
            </template>
          </v-sparkline>
          <div>
            Start time: {{ new Date(diagram.data[0].time) }}
          </div>
          <div>
            End time: {{ new Date(diagram.data[diagram.data.length - 1].time) }}
          </div>
        </v-sheet>
      </v-card-text>

      <v-card-text>
        <div class="text-h4 font-weight-thin">
          {{ diagram.name }}
        </div>
      </v-card-text>

      <v-divider></v-divider>
    </v-card>
  </v-content>
</template>

<script>
import axios from "axios";

export default {
  name: "Diagrams",
  data: () => ({
    mainServer: "http://localhost:8000",
    width: 2,
    radius: 10,
    padding: 8,
    lineCap: "round",
    value: [0, 2, 5, 9, 5, 10, 3, 5, 0, 0, 1, 8, 2, 9, 0],
    gradientDirection: "top",
    fill: false,
    type: "trend",
    autoLineWidth: false,
    resultsArray: []
  }),
  async mounted() {
    const IDs = ["sensor-0", "sensor-1", "actuator-0", "actuator-1"];
    for (const ID of IDs) {
      try {
        const response = await axios.get(this.mainServer + "/thing/data", {
          params: {ID},
          headers: {
            authorization: `Bearer ${localStorage.getItem("platform-token")}`
          }
        });
        console.log(response.data.data);
        const vals = [];
        for (const val of response.data.data) {
          if (val.val === "true" || val.val === "false")
            vals.push(val.val === "true" ? 1 : 0);
          else
            vals.push(Number.parseInt(val.val));
        }
        this.resultsArray.push({
          name: ID,
          data: response.data.data,
          vals
        });
      } catch (e) {
        console.log(e);
      }
    }

  }
};
</script>

<style scoped>

</style>
