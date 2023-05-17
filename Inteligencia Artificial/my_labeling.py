import numpy as np
import Kmeans as km
import KNN as kn
from utils_data import read_dataset, visualize_k_means, visualize_retrieval
import matplotlib.pyplot as plt
import timeit
#import cv2 as cv

#dummy setup
class setup:
    def __init__(self, k, K, n=None, alg="knnkm", options=None):
        train_imgs, train_class_labels, train_color_labels, \
        test_imgs, test_class_labels, test_color_labels = read_dataset(ROOT_FOLDER='./images/', gt_json='./images/gt.json')

        if  n == None or n <= 0 or n > 851:
            n = 851
        n2 = 1477 + n
        
        statistics = []
        km_labels = []
        num = 0
        #initialize dataset
        self.train_img = train_imgs[:n2]
        self.train_class_labels = train_class_labels[:n2]
        self.train_color_labels = train_color_labels[:n2]
        self.test_imgs = test_imgs
        self.test_class_labels = test_class_labels
        self.test_color_labels = test_color_labels
        self.classes = list(set(list(train_class_labels) + list(test_class_labels)))
        #initialize knn and kmeans
        if "knn" in alg:
            aux = kn.KNN(train_imgs, train_class_labels)
            self.knn = aux.predict(self.test_imgs, k)
        
        if "km" in alg:
            for i in test_imgs[:n]:
                start = timeit.default_timer()
                km_obj = km.KMeans(i, options=options)
                km_obj.find_bestK(K)
                stop = timeit.default_timer()
                statistics.append((i, km_obj, km_obj.threshold, km_obj.K, km_obj.num_iter, stop - start,  km_obj.wcd))
                km_labels.append(list(set(km.get_colors(km_obj.centroids))))
                num+=1
            
                self.km_labels = km_labels
                self.statistics = statistics
    
    
    def Retrieval_by_color(self, opt):
        if type(opt) is list:
            res = []
            for i,j in zip(self.test_imgs, self.km_labels):
                for k in opt: 
                    if k not in j: break
                else: res.append(i)       
            visualize_retrieval(res, len(res), title=" ".join(opt))
        
        if type(opt) is str: 
            res = [i for i,j in zip(self.test_imgs, self.km_labels) if opt in j]                
            visualize_retrieval(res, 20, title=opt) #20 porque sino hay tanta imagen que no se ve

    def Retrieval_by_shape(self, shpe):
        res = self.test_imgs[self.knn == shpe]
        visualize_retrieval(res, 20, title=shpe)

    def Retrieval_combined(self, str1, str2):
        res = [ i  for i,j,k in zip(self.test_imgs, self.knn, self.km_labels) 
            if j == str2 and str1 in k  ]
        visualize_retrieval(res, len(res), title=str1+" "+str2)
    
    def Kmean_statistics(self, verbose=True):
        num = 0
        mean_wcd = 0
        mean_k = 0
        mean_iter = 0
        mean_time = 0
        aux = len(self.statistics)
        for i in self.statistics: 
            if num%50==0 and verbose: 
                visualize_k_means(i[1], i[0].shape) 
                print("WCD: ", i[2], "K: ", i[3], "Number iteratios: ", i[4], "Execution time: ", i[5])
                plt.clf()
                plt.plot(range(2, len(i[6])+2), i[6])
                plt.show()
            mean_wcd  +=  i[2]
            mean_k    +=  i[3]
            mean_iter +=  i[4]
            mean_time +=  i[5]
            num+=1
        if verbose: print("WCD: ", mean_wcd/aux, "K: ", mean_k/aux, "Number iteratios: ", mean_iter/aux, "Execution time: ", mean_time/aux)
        return {"WCD":mean_wcd/aux, "K":mean_k/aux, "iter":mean_iter/aux, "time":mean_time/aux}
    
    def Get_shape_accuracy(self):
        aux = self.knn == self.test_class_labels
        #print("Shape accuracy: ", np.count_nonzero(aux)/aux.size)
        return np.count_nonzero(aux)/aux.size

    def Get_color_accuracy(self):
        aux = 0
        for i,j in zip(self.km_labels, self.test_color_labels):
            aux2 = set(j)
            aux += len(aux2.intersection(set(i)))/len(aux2)
        return aux/len(self.km_labels)
       


#funciones extras (ejecutar solo 1 vez son muy lentas)
#pruebas extras kmeans
def plotThreshold_stats(threshold):
    accuracy = []
    stats = []
    for i in range(1, threshold+1):
        prueba = setup(1, 100, 100, alg="km", options={'threshold':i})
        accuracy.append(prueba.Get_color_accuracy())
        stats.append(prueba.Kmean_statistics(False))
    
    plt.plot([i for i in range(1, threshold+1)], accuracy)
    plt.savefig("threshold_variation_accuracy.png")

    plt.clf()
    plt.plot([i for i in range(1, threshold+1)], [i['WCD'] for i in stats])
    plt.savefig("threshold_variation_wcd.png")
    
    plt.clf()
    plt.plot([i for i in range(1, threshold+1)], [i['K'] for i in stats])
    plt.savefig("threshold_variation_K.png")

    plt.clf()
    plt.plot([i for i in range(1, threshold+1)], [i['iter'] for i in stats])
    plt.savefig("threshold_variation_iter.png")

    plt.clf()
    plt.plot([i for i in range(1, threshold+1)], [i['time'] for i in stats])
    plt.savefig("threshold_variation_time.png")

def plotmethod_accuracy():
    prueba = setup(1, 100, 851, alg="km") #slow with large "n", third parameter max number 851
    prueba2 = setup(1, 100, 851, alg="km", options={'threshold':20, 'km_init':'random'})
    prueba3 = setup(1, 100, 851,  alg="km", options={'threshold':20, 'km_init':'custom'})

    print(prueba.Kmean_statistics(False), prueba.Get_color_accuracy())
    print(prueba2.Kmean_statistics(False), prueba2.Get_color_accuracy())
    print(prueba3.Kmean_statistics(False), prueba3.Get_color_accuracy())
    
#pruebas extras knn
def plotAccuracy_predict(K):
    accuracy = []
    for i in range(1, K+1, 3):
        prueba = setup(i, 100, 851,alg="knn")
        accuracy.append(prueba.Get_shape_accuracy())
    plt.plot([i for i in range(1, K+1, 3)], accuracy)
    plt.savefig("K_predict_variation.png")

def plotAccuracy_trainingSet(n):
    accuracy = []
    for i in range(1, n, 50):
        prueba = setup(1, 100, i, alg="knn")
        accuracy.append(prueba.Get_shape_accuracy())
    plt.plot([i for i in range(1, n, 50)], accuracy)
    plt.savefig("training_predict_variation.png")

if __name__ == '__main__':
    plotThreshold_stats(33)
    plotAccuracy_predict(50)
    plotAccuracy_trainingSet(851)
    plotmethod_accuracy()
    prueba = setup(1, 100, 851, alg="km") #slow with large "n", third parameter max number 851
    prueba.Retrieval_by_color(["Red", "Green", "Blue"])
    prueba.Retrieval_by_color("Yellow")
    prueba.Retrieval_by_shape(np.random.choice(prueba.classes))
    prueba.Retrieval_combined("Green", np.random.choice(prueba.classes))
    prueba.Get_shape_accuracy()
    prueba.Kmean_statistics()
    print(prueba.Get_color_accuracy())
