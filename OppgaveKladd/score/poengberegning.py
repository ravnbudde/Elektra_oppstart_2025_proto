import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.backends.backend_pdf import PdfPages
from datetime import datetime

class PoengBeregner:
    def __init__(self, csv_fil='resultater.csv'):
        """
        Initialiserer poengberegner med CSV-fil
        """
        self.df = pd.read_csv(csv_fil)
        
        # Konfigurasjon for hver oppgave
        self.oppgave_config = {
            'del1.1': {
                'navn': 'Del 1.1 - Fartsregulering',
                'enhet': 'sekunder',
                'lavere_bedre': True,  # True = lavere verdi er bedre (tid)
                'vekt': 1.0,  # Skalering av poeng
                'diskvalifisering': 30.0  # Maks tillatt tid
            },
            'del1.2': {
                'navn': 'Del 1.2 - Forbikjøring',
                'enhet': 'sekunder',
                'lavere_bedre': True,
                'vekt': 1.0,
                'diskvalifisering': 60.0
            },
            'del2': {
                'navn': 'Del 2',
                'enhet': 'poeng',
                'lavere_bedre': False,  # False = høyere verdi er bedre
                'vekt': 1.5,  # Vektet høyere
                'diskvalifisering': 0.0  # Min tillatt verdi
            },
            'del3': {
                'navn': 'Del 3',
                'enhet': 'sekunder',
                'lavere_bedre': True,
                'vekt': 2.0,  # Dobbel vekting
                'diskvalifisering': 120.0
            }
        }
        
    def chi2_cdf_approx(self, x, df=3):
        """
        Enkel approksimasjon av chi2 CDF uten scipy
        Bruker gamma-funksjon approksimasjon
        """
        # Sikre at x er mellom 0 og 1
        x = np.clip(x, 0, 1)
        
        # Enkel approksimasjon som gir S-kurve lignende chi2
        # Dette gir progressiv poengfordeling
        return 1 - np.exp(-x * df / 2)
        
    def beregn_poeng_chi2(self, verdier, lavere_bedre=True, diskvalifisering=None):
        """
        Beregner poeng basert på chi2-lignende fordeling
        """
        # Fjern diskvalifiserte resultater
        if diskvalifisering is not None:
            if lavere_bedre:
                gyldige = verdier[verdier <= diskvalifisering]
            else:
                gyldige = verdier[verdier >= diskvalifisering]
        else:
            gyldige = verdier
        
        if len(gyldige) == 0:
            return pd.Series([0] * len(verdier), index=verdier.index)
        
        # Normaliser verdiene
        if lavere_bedre:
            # For tid: beste (minste) = 1, dårligste = 0
            min_val = gyldige.min()
            max_val = gyldige.max()
            if max_val > min_val:
                normalisert = 1 - (verdier - min_val) / (max_val - min_val)
            else:
                normalisert = pd.Series([1] * len(verdier), index=verdier.index)
        else:
            # For poeng: beste (største) = 1, dårligste = 0
            min_val = gyldige.min()
            max_val = gyldige.max()
            if max_val > min_val:
                normalisert = (verdier - min_val) / (max_val - min_val)
            else:
                normalisert = pd.Series([1] * len(verdier), index=verdier.index)
        
        # Sett diskvalifiserte til 0
        if diskvalifisering is not None:
            if lavere_bedre:
                normalisert[verdier > diskvalifisering] = 0
            else:
                normalisert[verdier < diskvalifisering] = 0
        
        # Bruk chi2-lignende fordeling for å justere poengene
        chi2_verdier = self.chi2_cdf_approx(normalisert)
        
        return chi2_verdier
    
    def beregn_alle_poeng(self):
        """
        Beregner poeng for alle oppgaver
        """
        resultater = self.df.copy()
        
        for oppgave, config in self.oppgave_config.items():
            poeng_kolonne = f'{oppgave}_poeng'
            resultater[poeng_kolonne] = self.beregn_poeng_chi2(
                self.df[oppgave],
                lavere_bedre=config['lavere_bedre'],
                diskvalifisering=config.get('diskvalifisering')
            )
            
            # Skaler med vekt
            resultater[poeng_kolonne] *= config['vekt']
        
        # Legg til bonus
        resultater['bonus_skalert'] = self.df['bonus']
        
        # Beregn totalpoeng
        poeng_kolonner = [f'{opp}_poeng' for opp in self.oppgave_config.keys()]
        poeng_kolonner.append('bonus_skalert')
        resultater['totalpoeng'] = resultater[poeng_kolonner].sum(axis=1)
        
        # Sorter etter totalpoeng
        resultater = resultater.sort_values('totalpoeng', ascending=False)
        resultater['plassering'] = range(1, len(resultater) + 1)
        
        return resultater
    
    def vis_resultater(self, resultater):
        """
        Viser resultater i konsollen med tydelige headers
        """
        print("\n" + "="*100)
        print("RESULTATER - OPPSTARTSUKA BIAIS 2025")
        print("="*100)
        
        print("\nTOPP 5 GRUPPER:")
        print("-"*100)
        topp5 = resultater.head(5)[['gruppeNr', 'totalpoeng', 'plassering']]
        for idx, row in topp5.iterrows():
            plassering = int(row['plassering'])
            gruppe_nr = int(row['gruppeNr'])
            print(f"{plassering:2d}. plass: Gruppe {gruppe_nr:2d} - {row['totalpoeng']:.2f} poeng")
        
        print("\n\nDETALJERT POENGFORDELING:")
        print("-"*100)
        print("\nFORKLARING:")
        print("- Kolonner med oppgavenavn (del1.1, del1.2, etc.) = MÅLT VERDI (tid/score)")
        print("- Kolonner med '_poeng' = BEREGNET POENG (0-1 skalert med vekt)")
        print("-"*100)
        
        # Konverter til int for visning
        resultater_vis = resultater.copy()
        resultater_vis['plassering'] = resultater_vis['plassering'].astype(int)
        resultater_vis['gruppeNr'] = resultater_vis['gruppeNr'].astype(int)
        
        # Lag header med enheter
        headers = []
        for col in ['gruppeNr', 'plassering']:
            headers.append(col)
        
        for opp, config in self.oppgave_config.items():
            headers.append(f"{opp}\n({config['enhet']})")
            headers.append(f"{opp}_poeng\n(vekt: {config['vekt']})")
        
        headers.extend(['bonus', 'totalpoeng'])
        
        # Vis med custom headers
        vis_kolonner = ['gruppeNr', 'plassering']
        for opp in self.oppgave_config.keys():
            vis_kolonner.extend([opp, f'{opp}_poeng'])
        vis_kolonner.extend(['bonus', 'totalpoeng'])
        
        pd.set_option('display.max_columns', None)
        pd.set_option('display.width', None)
        pd.set_option('display.float_format', '{:.2f}'.format)
        print(resultater_vis[vis_kolonner].to_string(index=False))
    
    def lag_pdf_rapport(self, resultater, filnavn='resultater_rapport.pdf'):
        """
        Lager en omfattende PDF-rapport med alle resultater og visualiseringer
        """
        print("\nGenererer PDF-rapport...")
        
        with PdfPages(filnavn) as pdf:
            # Side 1: Forside og sammendrag
            fig = plt.figure(figsize=(8.5, 11))
            fig.suptitle('BIAIS Oppstartsuka 2025\nResultatrapport', fontsize=24, y=0.95)
            
            # Tekst med sammendrag
            ax = fig.add_subplot(111)
            ax.axis('off')
            
            sammendrag_tekst = f"""
            Dato: {datetime.now().strftime('%d.%m.%Y')}
            
            Antall grupper: {len(resultater)}
            
            TOPP 5 GRUPPER:
            """
            
            topp5 = resultater.head(5)
            for idx, row in topp5.iterrows():
                sammendrag_tekst += f"\n{int(row['plassering'])}. plass: Gruppe {int(row['gruppeNr'])} - {row['totalpoeng']:.2f} poeng"
            
            ax.text(0.1, 0.7, sammendrag_tekst, fontsize=14, verticalalignment='top')
            
            pdf.savefig(fig, bbox_inches='tight')
            plt.close()
            
            # Side 2: Totalresultater oversikt
            fig, axes = plt.subplots(2, 2, figsize=(11, 8.5))
            fig.suptitle('Totalresultater - Oversikt', fontsize=16)
            
            # Plot 1: Totalpoeng per gruppe (stolpediagram)
            ax1 = axes[0, 0]
            grupper = resultater.sort_values('gruppeNr')
            bars = ax1.bar(grupper['gruppeNr'], grupper['totalpoeng'], color='steelblue')
            
            # Marker topp 3
            topp3 = resultater.head(3)
            for idx, row in topp3.iterrows():
                gruppe_idx = list(grupper['gruppeNr']).index(row['gruppeNr'])
                bars[gruppe_idx].set_color('gold' if row['plassering'] == 1 else 
                                          'silver' if row['plassering'] == 2 else 
                                          'chocolate')
            
            ax1.set_xlabel('Gruppenummer')
            ax1.set_xticks(grupper['gruppeNr'])
            ax1.set_xticklabels([f'{int(g)}' for g in grupper['gruppeNr']])
            ax1.set_ylabel('Totalpoeng')
            ax1.set_title('Totalpoeng per gruppe')
            ax1.grid(True, alpha=0.3)
            
            # Plot 2: Histogram av totalpoeng
            ax2 = axes[0, 1]
            ax2.hist(resultater['totalpoeng'], bins=10, color='forestgreen', alpha=0.7, edgecolor='black')
            ax2.axvline(resultater['totalpoeng'].mean(), color='red', linestyle='--', 
                       label=f'Gjennomsnitt: {resultater["totalpoeng"].mean():.2f}')
            ax2.set_xlabel('Totalpoeng')
            ax2.set_ylabel('Antall grupper')
            ax2.set_title('Fordeling av totalpoeng')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
            
            # Plot 3: Box plot av alle oppgaver
            ax3 = axes[1, 0]
            poeng_data = []
            labels = []
            for opp, config in self.oppgave_config.items():
                poeng_data.append(resultater[f'{opp}_poeng'])
                labels.append(config['navn'].split(' - ')[-1] if ' - ' in config['navn'] else config['navn'])
            
            box_plot = ax3.boxplot(poeng_data, labels=labels, patch_artist=True)
            for patch in box_plot['boxes']:
                patch.set_facecolor('lightblue')
            
            ax3.set_ylabel('Poeng')
            ax3.set_title('Poengfordeling per oppgave')
            ax3.tick_params(axis='x', rotation=45)
            ax3.grid(True, alpha=0.3)
            
            # Plot 4: Kumulativ fordeling
            ax4 = axes[1, 1]
            sorted_poeng = np.sort(resultater['totalpoeng'])
            y = np.arange(1, len(sorted_poeng) + 1) / len(sorted_poeng)
            ax4.plot(sorted_poeng, y, 'b-', linewidth=2)
            ax4.set_xlabel('Totalpoeng')
            ax4.set_ylabel('Kumulativ sannsynlighet')
            ax4.set_title('Kumulativ fordeling av totalpoeng')
            ax4.grid(True, alpha=0.3)
            
            plt.tight_layout()
            pdf.savefig(fig, bbox_inches='tight')
            plt.close()
            
            # Side 3 og fremover: Detaljert analyse per oppgave
            for oppgave, config in self.oppgave_config.items():
                fig, axes = plt.subplots(2, 2, figsize=(11, 8.5))
                fig.suptitle(f'Detaljert analyse: {config["navn"]}', fontsize=16)
                
                # Plot 1: Rådata vs poeng (scatter)
                ax1 = axes[0, 0]
                verdier = resultater[oppgave]
                poeng = resultater[f'{oppgave}_poeng']
                
                scatter = ax1.scatter(verdier, poeng, alpha=0.6, s=50, c=poeng, cmap='viridis')
                ax1.set_xlabel(f'{config["enhet"]}')
                ax1.set_ylabel('Poeng')
                ax1.set_title('Rådata vs Poeng')
                ax1.grid(True, alpha=0.3)
                plt.colorbar(scatter, ax=ax1)
                
                # Plot 2: Histogram av rådata
                ax2 = axes[0, 1]
                ax2.hist(verdier, bins=15, color='skyblue', alpha=0.7, edgecolor='black')
                ax2.axvline(verdier.mean(), color='red', linestyle='--', 
                           label=f'Gjennomsnitt: {verdier.mean():.2f}')
                ax2.axvline(verdier.median(), color='green', linestyle='--', 
                           label=f'Median: {verdier.median():.2f}')
                ax2.set_xlabel(config['enhet'])
                ax2.set_ylabel('Antall grupper')
                ax2.set_title(f'Fordeling av {config["navn"]}')
                ax2.legend()
                ax2.grid(True, alpha=0.3)
                
                # Plot 3: Chi2-fordeling visualisering
                ax3 = axes[1, 0]
                x_vals = np.linspace(0, 1, 100)
                y_vals = self.chi2_cdf_approx(x_vals)
                ax3.plot(x_vals, y_vals, 'b-', linewidth=2, label='Chi2-fordeling')
                ax3.set_xlabel('Normalisert verdi')
                ax3.set_ylabel('Poeng')
                ax3.set_title('Chi2-fordelingsfunksjon')
                ax3.grid(True, alpha=0.3)
                ax3.legend()
                
                # Plot 4: Topp og bunn grupper
                ax4 = axes[1, 1]
                sorted_res = resultater.sort_values(oppgave, ascending=config['lavere_bedre'])
                topp5 = sorted_res.head(5)
                bunn5 = sorted_res.tail(5)
                
                y_pos = np.arange(10)
                grupper = list(topp5['gruppeNr']) + list(bunn5['gruppeNr'])
                verdier_plot = list(topp5[oppgave]) + list(bunn5[oppgave])
                farger = ['green']*5 + ['red']*5
                
                bars = ax4.barh(y_pos, verdier_plot, color=farger, alpha=0.7)
                ax4.set_yticks(y_pos)
                ax4.set_yticklabels([f'Gr. {int(g)}' for g in grupper])
                ax4.set_xlabel(config['enhet'])
                ax4.set_title('Topp 5 (grønn) og Bunn 5 (rød)')
                ax4.grid(True, alpha=0.3, axis='x')
                
                # Legg til verdier på stolpene
                for i, v in enumerate(verdier_plot):
                    ax4.text(v + 0.01*max(verdier_plot), i, f'{v:.1f}', va='center')
                
                plt.tight_layout()
                pdf.savefig(fig, bbox_inches='tight')
                plt.close()
            
            # Siste side: Detaljert tabell
            fig = plt.figure(figsize=(11, 8.5))
            ax = fig.add_subplot(111)
            ax.axis('tight')
            ax.axis('off')

            # Forbered data for tabell - ALLE grupper
            tabell_data = []
            for idx, row in resultater.iterrows():
                rad = [
                    int(row['plassering']),
                    int(row['gruppeNr']),
                    f"{row['totalpoeng']:.2f}"
                ]
                for opp in self.oppgave_config.keys():
                    rad.append(f"{row[opp]:.1f}")
                    rad.append(f"{row[f'{opp}_poeng']:.2f}")
                tabell_data.append(rad)

            # Lag kolonneoverskrifter
            kolonner = ['Plass', 'Gruppe', 'Total']
            for opp, config in self.oppgave_config.items():
                kolonner.append(f"{opp}\n({config['enhet']})")
                kolonner.append(f"Poeng\n(×{config['vekt']})")

            # Lag tabell med ALLE grupper
            tabell = ax.table(cellText=tabell_data,
                            colLabels=kolonner,
                            cellLoc='center',
                            loc='center')

            tabell.auto_set_font_size(False)
            tabell.set_fontsize(7)
            tabell.scale(1, 1.2)

            # Fargelegg header
            for i in range(len(kolonner)):
                tabell[(0, i)].set_facecolor('#40466e')
                tabell[(0, i)].set_text_props(weight='bold', color='white')

            # Først fargelegg topp 3 per oppgave (lysere farger)
            farger_oppgave = ['#FFFACD', '#F0F0F0', '#FFDAB9']  # Lysere farger
            for opp_idx, oppgave in enumerate(self.oppgave_config.keys()):
                # Finn topp 3 for denne oppgaven
                sorted_opp = resultater.sort_values(oppgave, 
                                                ascending=self.oppgave_config[oppgave]['lavere_bedre'])
                topp3_grupper = sorted_opp.head(3)['gruppeNr'].values
                
                # Finn kolonne-indeks for oppgaven
                kol_idx = 3 + opp_idx * 2  # Rådata-kolonnen
                
                # Fargelegg topp 3 for denne oppgaven
                for rad_idx, rad in enumerate(tabell_data):
                    gruppe_nr = rad[1]  # Gruppenummer er i kolonne 1
                    if gruppe_nr in topp3_grupper:
                        rang = list(topp3_grupper).index(gruppe_nr)
                        if rang < 3:
                            tabell[(rad_idx+1, kol_idx)].set_facecolor(farger_oppgave[rang])
                            tabell[(rad_idx+1, kol_idx+1)].set_facecolor(farger_oppgave[rang])

            # SÅ fargelegg topp 3 total (overskriver hele raden)
            farger_total = ['#FFD700', '#C0C0C0', '#CD7F32']  # Gull, sølv, bronse
            for i in range(min(3, len(tabell_data))):
                for j in range(3):  # Bare fargelegg plass, gruppe og total kolonner
                    tabell[(i+1, j)].set_facecolor(farger_total[i])

            ax.set_title('Detaljert resultattabell (Alle grupper)\nGull/Sølv/Bronse = Total | Lys gul/grå/fersken = Topp 3 per oppgave', 
                        fontsize=14, pad=20)

            pdf.savefig(fig, bbox_inches='tight')
            plt.close()
        
        print(f"PDF-rapport lagret som: {filnavn}")
    
    def eksporter_resultater(self, resultater, filnavn='resultater_med_poeng.csv'):
        """
        Eksporterer resultater til CSV
        """
        resultater_eks = resultater.copy()
        resultater_eks['plassering'] = resultater_eks['plassering'].astype(int)
        resultater_eks['gruppeNr'] = resultater_eks['gruppeNr'].astype(int)
        
        resultater_eks.to_csv(filnavn, index=False, float_format='%.2f')
        print(f"Resultater eksportert til: {filnavn}")

# Hovedprogram
if __name__ == "__main__":
    try:
        # Opprett beregner
        beregner = PoengBeregner('resultater.csv')
        
        # Beregn poeng
        resultater = beregner.beregn_alle_poeng()
        
        # Vis resultater i terminal
        beregner.vis_resultater(resultater)
        
        # Generer PDF-rapport
        beregner.lag_pdf_rapport(resultater)
        
        # Eksporter til CSV
        beregner.eksporter_resultater(resultater)
        
    except FileNotFoundError:
        print("Feil: Finner ikke 'resultater.csv'. Sjekk at filen ligger i samme mappe som scriptet.")
    except Exception as e:
        print(f"En feil oppstod: {e}")
        import traceback
        traceback.print_exc()